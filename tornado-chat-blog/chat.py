#!/usr/bin/env python
# -*- coding:utf-8 -*-

import os
import tornado.httpserver
import tornado.ioloop
import tornado.options
import tornado.web

import pymongo
import datetime
import time
import markdown2
import sys
reload(sys)
sys.setdefaultencoding('utf-8')

from tornado.options import define, options
define("port", default=8888, help="run on the given port", type=int)


class baseHandler(tornado.web.RequestHandler):
    def get_current_user(self):
        return self.get_secure_cookie("username")

class registerHandler(baseHandler):
    def get(self):
        self.render('register.html')
    def post(self):
        name = self.get_argument('username','admin')
        pw = self.get_argument('password','admin')
        res = self.check(name)
        if res:
            self.render('register1.html')
            #self.redirect('/register')
        else:
            userlist = self.application.db.user
            now = datetime.datetime.now()
            jointime = now.strftime('%Y-%m-%d %H:%M:%S')
            userlist.insert({'username':name,'password':pw,'articleList':[],'commentList':[],'joinTime':jointime})
            self.set_secure_cookie('username',name)
            self.redirect('/')
    def check(self,name):
        userlist = self.application.db.user
        if userlist.find_one({'username':name}):
            return True
        return False


class indexHandler(baseHandler):
    def get(self):
        name = self.current_user
        blogs = self.showAllBlog()
        self.render('index.html',cookieName=name,blogs=blogs)
    def post(self):
        name = self.get_argument('username')
        pw = self.get_argument('password')
        res = self.check(name,pw)
        if res:
            self.set_secure_cookie('username',name)
        else:
            pass
        #self.redirect('/')
        self.redirect(self.get_argument('next','/'))
    def check(self,name,pw=None):
        userlist = self.application.db.user
        if userlist.find_one({'username':name}):
            if(userlist.find_one({'username':name}))['password']==pw:
                return True
        return False
    def showAllBlog(self):
        articlelist = self.application.db.article
        return articlelist.find().sort("articleId",pymongo.ASCENDING)


class menberHandler(baseHandler):
    def get(self):
        name = self.current_user
        users = self.show()
        users.sort('joinTime',pymongo.ASCENDING)
        self.render('member.html',cookieName=name,users=users)
    def show(self):
        userlist = self.application.db.user
        userlist = userlist.find()
        return userlist


class postHandler(baseHandler):
    @tornado.web.authenticated
    def get(self):
        name = self.current_user
        self.render('post.html',cookieName=name)
    @tornado.web.authenticated
    def post(self):
        title = self.get_argument('title')
        blog_md = self.get_argument('blog')
        blog = self.translate(blog_md)
        name = self.current_user
        idvalue = self.insertBlog(name,title,blog)
        self.redirect('/blog/'+str(idvalue))
    def insertBlog(self,name,title,blog):
        now = datetime.datetime.now()
        posttime = now.strftime('%Y-%m-%d %H:%M:%S')
        idvalue = now.strftime('%Y%m%d%H%M%S')
        article = self.application.db.article
        owner = self.current_user
        user = self.application.db.user.find_one({'username':owner})
        user['articleList'].append(idvalue)
        self.application.db.user.save(user)
        article.insert({'articleId':idvalue,'owner':owner,'title':title,'content':blog,'postTime':posttime,'comment':[]})
        return idvalue
    def translate(self,md):
        md = md.decode('utf-8')
        md = markdown2.markdown(md)
        return md.encode('UTF-8')


class blogHandler(baseHandler):
    def get(self,idvalue):
        selfname = self.current_user
        blog = self.application.db.article.find_one({'articleId':idvalue})
        comments = self.showComment(idvalue)
        self.render('blog.html',cookieName=selfname,blog=blog,comments=comments)
    def showComment(self,idvalue):
        commentList = self.application.db.comment.find({'articleId':idvalue})
        return commentList

class commentHandler(baseHandler):
    @tornado.web.authenticated
    def post(self,blogid):
        selfname = self.current_user
        comment = self.get_argument('comment')
        #refer = self.request.handers.get('refer')
        #for i in range(len(refer)-1,0,-1):
        #    if refer[i]=='/':
        #        break
        #blogid = refer[i+1:]
        #print blogid
        self.addComment(blogid,selfname,comment)
        self.redirect('/blog/'+blogid)
    def addComment(self,blogid,selfname,comment):
        article = self.application.db.article.find_one({'articleId':blogid})
        user = self.application.db.user.find_one({'username':selfname})
        comments = self.application.db.comment
        now = datetime.datetime.now()
        commentId = now.strftime('%Y%m%d%H%M%S')
        posttime = now.strftime('%Y-%m-%d %H:%M:%S')
        comments.insert({'commentId':commentId,'articleId':blogid,'owner':selfname,'content':comment,'postTime':posttime})
        article['comment'].append(commentId)
        self.application.db.article.save(article)
        user['commentList'].append(commentId)
        self.application.db.user.save(user)


class userHandler(baseHandler):
    def get(self,input):
        selfname = self.current_user
        user = self.application.db.user.find_one({'username':input})
        articleList = self.application.db.article.find({'owner':input})
        commentList = self.application.db.comment.find({'owner':input})
        self.render('user.html',cookieName=selfname,user=user,articleList=articleList,commentList=commentList)


class chatHandler(baseHandler):
    def get(self):
        selfname = self.current_user
        group = self.application.db.groupchat.find().sort('chatId',pymongo.ASCENDING)
        self.render('chat.html',group=group,cookieName=selfname)
    def post(self):
        selfname = self.current_user
        topic = self.get_argument('topic',None)
        password = self.get_argument('password',None)
        now = datetime.datetime.now()
        topicId = now.strftime('%Y%m%d%H%M%S')
        posttime = now.strftime('%Y-%m-%d %H:%M:%S')
        peopleList = []
        peopleList.append(selfname)
        if len(topic)<2 or len(password)<2:
            self.redirect('/chat')
        else:
            self.application.db.groupchat.insert({'chatId':topicId,'owner':selfname,'password':password,'title':topic,'peopleList':peopleList,'msgList':[],'postTime':posttime})
            self.redirect('/chat')

class chatingHandler(baseHandler):
    def get(self,idvalue):
        selfname = self.current_user
        password = self.get_argument('password',"")
        topic = self.application.db.groupchat.find_one({'chatId':idvalue})
        for i in range(len(topic['peopleList'])):
            if selfname==topic['peopleList'][i]:
                break
        if i==len(topic['peopleList'])-1:
            topic['peopleList'].append(selfname)
            self.application.db.groupchat.save(topic)
        del topic["_id"]
        if password==topic['password']:
            self.render('chating.html',topic=topic,cookieName=selfname)
        else:
            self.write('密码错误，请重新输入')
    def post(self,idvalue):
        selfname = self.current_user
        topic = self.application.db.groupchat.find_one({'chatId':idvalue})
        msg = self.get_argument('msg',"")
        if msg != "":
            topic['msgList'].append([selfname,datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S'),msg])
        self.application.db.groupchat.save(topic)
        del topic["_id"]
        self.write(topic)

class delChatHandler(baseHandler):
    def post(self):
        try:
            selfname = self.current_user
            idvalue = self.get_argument('idvalue',None)
            topic = self.application.db.groupchat
            topic.remove({'chatId':idvalue})
            self.write('true')
        except:
            self.write('false')

class logoutHandler(baseHandler):
    @tornado.web.authenticated
    def get(self):
        self.clear_cookie('username')
        self.redirect('/')

class Application(tornado.web.Application):
    def __init__(self):
        handlers = [
                (r'/',indexHandler),
                (r'/member',menberHandler),
                (r'/chat',chatHandler),
                (r'/chating/(\d+)',chatingHandler),
                (r'/delChat',delChatHandler),
                (r'/register',registerHandler),
                (r'/logout',logoutHandler),
                (r'/post',postHandler),
                (r'/user/(\w+)',userHandler),
                (r'/blog/(\d+)',blogHandler),
                (r'/comment/(\d+)',commentHandler)
                ]
        settings = {
                'template_path':'templates',
                'static_path':'static',
                'cookie_secret':'2j0VRNUkTOuT3DRnvcv7js5Izxw/X0uCnOIIXZgW8bI=',
                'login_url':'/',
                #'xsrf_cookies':True,
                'autoescape':None,
                'debug':True
                }
        conn = pymongo.Connection('localhost',27017)
        self.db = conn['chat']
        tornado.web.Application.__init__(self,handlers,**settings)

if __name__ == "__main__":
    tornado.options.parse_command_line()
    http_server = tornado.httpserver.HTTPServer(Application())
    http_server.listen(options.port)
    tornado.ioloop.IOLoop.instance().start()

