#!/usr/bin/env python
# -*- coding:utf-8 -*-

import spider
import tornado.web
import tornado.httpserver
import tornado.options
import tornado.ioloop

from datetime import *
import pymongo

from tornado.options import define,options
define("port",default=8888,help="run on the given port",type=int)

class BaseHandler(tornado.web.RequestHandler):
    def get_current_user(self):
        return self.get_secure_cookie("username")

class LoginHandler(BaseHandler):
    def get(self):
        self.render("login.html")
    def post(self):
        username = self.get_argument('username','SB')
        password = self.get_argument('password','jb')
        result, cookie = spider.Login(username,password)
        if result:
            self.set_secure_cookie("username",username)
            #这里需要判断是老师还是学生,暂时只用学生帐号，以后有更改
            coll = self.application.db.students
            student = coll.find_one({"number":username})
            if student:
                student["cookie"] = cookie
                coll.save(student)
            else:
                coll.insert({"number":username,"cookie":cookie})
            self.redirect("/")
            print username,'+++++++',password
        else:
            self.render("login1.html")


class IndexHandler(BaseHandler):
    @tornado.web.authenticated
    def get(self):
        coll = self.application.db.students
        student = coll.find_one({"number":self.current_user})
        result,name = spider.get_info(student["cookie"])
        student["name"] = name
        if student.has_key("tasks")==0:
            student["tasks"]={}
        coll.save(student)
        self.render("index.html",user=self.current_user,name=name[0],tasks=student['tasks'])

    @tornado.web.authenticated
    def post(self):
        coll = self.application.db.students
        student = coll.find_one({"number":self.current_user})
        taskName = self.get_argument('newtask','')
        startTime = date.today()
        endTime = self.get_argument('endtime','')
        taskinfo={"startTime":str(startTime),"endTime":endTime,"discription":''}
        if student.has_key('tasks')==0:
            student["tasks"]={}
        student["tasks"][taskName]={}
        student["tasks"][taskName]=taskinfo
        if taskName!= '' and endTime!='':
            coll.save(student)
        #删除
        delete = 'none'
        delete = self.get_argument('delete','none')
        print delete
#        if delete == "delete+":
#            if student['tasks'].has_key(u''):
#                del student['tasks'][u'']
        if delete!="none" and delete!='delete+':
            del_task = delete.split('+')[1]
            del student["tasks"][del_task]
            coll.save(student)
            delete = 'none'
        if delete == "delete+" or delete=='none':
            if student['tasks'].has_key(u''):
                del student['tasks'][u'']
                coll.save(student)
        self.render("index.html",user=self.current_user,name=student["name"][0],tasks=student["tasks"])

class LogoutHandler(BaseHandler):
    @tornado.web.authenticated
    def get(self):
        self.clear_cookie("username")
        self.redirect("/login")

class Application(tornado.web.Application):
    def __init__(self):
        handlers = [
                (r'/login',LoginHandler),
                (r'/logout',LogoutHandler),
                (r'/',IndexHandler)
                ]
        settings = {
                'template_path':'templates',
                'static_path':'static',
                'cookie_secret':'2j0VRNUkTOuT3DRnvcv7js5Izxw/X0uCnOIIXZgW8bI=',
                #'xsrf_cookies':True,
                'login_url':'/login',
                'debug':True
                }
        conn = pymongo.Connection('localhost',27017)
        self.db = conn["DeadlineKiller"]
        tornado.web.Application.__init__(self,handlers,**settings)

if __name__=="__main__":
    tornado.options.parse_command_line()
    server = tornado.httpserver.HTTPServer(Application())
    server.listen(options.port)
    tornado.ioloop.IOLoop.instance().start()

