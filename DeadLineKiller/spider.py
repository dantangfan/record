#!/usr/bin/env python
#-*- coding:utf-8 -*-

import urllib
import urllib2
import pycurl
import StringIO
import cookielib
import re
import socket
socket.setdefaulttimeout(10)


login_url = 'http://uems.sysu.edu.cn/jwxt/j_unieap_security_check.do'
mainPage_url = 'http://uems.sysu.edu.cn/jwxt/edp/index.jsp'
headers = ({
    'User-Agent':'Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/34.0.1847.132 Safari/537.36'
    })


def Login(userName,passwd):
    login_url = 'http://uems.sysu.edu.cn/jwxt/j_unieap_security_check.do'
    mainPage_url = 'http://uems.sysu.edu.cn/jwxt/edp/index.jsp'
    #headers = ({
    #'User-Agent':'Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/34.0.1847.132 Safari/537.36'
    #})

    postdata = urllib.urlencode({
        'j_username':userName,
        'j_password':passwd
        })
    cookie = cookielib.CookieJar()
    opener = urllib2.build_opener(urllib2.HTTPCookieProcessor(cookie),urllib2.HTTPHandler)

    request = urllib2.Request(login_url,data = postdata, headers = headers)
    result = opener.open(request)

    mainPage = opener.open(mainPage_url).read()
    if len(re.findall('中山大学'.decode('utf8'),mainPage.decode('utf8')))==0:
        return False,'Error'
    else:
        value = ''
        for item in cookie: value = item.value
        return True, "JSESSIONID="+value




def get_data(url,cookie,jsonForm):
    request = pycurl.Curl()
    request.setopt(pycurl.URL,url)
    request.setopt(pycurl.TIMEOUT,10)
    string = StringIO.StringIO()
    request.setopt(pycurl.WRITEFUNCTION,string.write)
    request.setopt(pycurl.POST,True)
    request.setopt(pycurl.POSTFIELDS,jsonForm)
    request.setopt(pycurl.COOKIE,cookie)
    request.setopt(pycurl.HTTPHEADER,['Content-Type: multipart/form-data', 'render: unieap'])
    try:
        request.perform()
    except:
        return False,'TIMEOUT'
    value = string.getvalue()
    request.close()
    if (value.startswith('THE-NODE-OF-SESSION-TIMEOUT',5)):
        return False,'Unexcepted Error'
    else:
        return True, value



#get name and major
def get_info(cookie):
    url = "http://uems.sysu.edu.cn/jwxt/WhzdAction/WhzdAction.action?method=getGrwhxxList"
    query_json = """
    {
        header: {
            "code": -100,
            "message": {
                "title": "",
                "detail": ""
            }
        },
        body: {
            dataStores: {
                xsxxStore: {
                    rowSet: {
                        "primary": [],
                        "filter": [],
                        "delete": []
                    },
                    name: "xsxxStore",
                    pageNumber: 1,
                    pageSize: 10,
                    recordCount: 0,
                    rowSetName: "pojo_com.neusoft.education.sysu.xj.grwh.model.Xsgrwhxx"
                }
            },
            parameters: {
                "args": [""]
            }
        }
    }
    """
    #return get_data(url, cookie, query_json)
    result, data = get_data(url,cookie,query_json)
    if result==False:
        return False, "Error"
    else:
        xm=[]
        #xm.append(data[data.find('"xm":"')+6:data.find('","xmpy":')])
        #xm.append(data[data.find('","bjmc":')+10:data.find('","xyh":')])
        rest = data[data.find('"xm":"')+6:-1]
        xm.append(rest[0:rest.find('","')])
        return True ,xm



#get course list
def get_courselist(cookie,year,term):
    url = 'http://uems.sysu.edu.cn/jwxt/xstk/xstk.action?method=getXkxkjglistByxh'
    query_json = """
    {
        header: {
            "code": -100,
            "message": {
                "title": "",
                "detail": ""
            }
        },
        body: {
            dataStores: {
                xsxkjgStore: {
                    rowSet: {
                        "primary": [],
                        "filter": [],
                        "delete": []
                    },
                    name: "xsxkjgStore",
                    pageNumber: 1,
                    pageSize: 100,
                    recordCount: 0,
                    rowSetName: "pojo_com.neusoft.education.sysu.xk.drxsxkjg.entity.XkjgEntity",
                    order: "xnd desc,xq desc,kclbm asc"
                }
            },
            parameters: {
                "xsxkjgStore-params": [
                    {
                        "name": "xnd",
                        "type": "String",
                        "value": "'%s'",
                        "condition": " = ",
                        "property": "xnd"
                    },
                    {
                        "name": "xq",
                        "type": "String",
                        "value": "'%s'",
                        "condition": " = ",
                        "property": "xq"
                    }
                ],
                "args": []
            }
        }
    }
    """ %(year, term)
    result, data =  get_data(url, cookie, query_json)
    course = []
    while data.find('kcmc":"')!=-1:
        course.append(data[data.find('kcmc":"')+7:data.find('","zjjs')])
        data = data[data.find('","zjjs')+50:-1]
        print len(data)
    if result==True:
        return True,course
    else: return False,'timeout'


if __name__=="__main__":
    result,cookie  = Login('11331130','09192970')
    if(result==True):
        #result,msg = get_info(cookie)
        #if(result==True):
        #    print msg[0],msg[1]
        result,l = get_courselist(cookie,'2013-2014','3')
        if (result==True):
            for i in range(len(l)):
                print l[i]
        else:print 'fuck'
    #print cookie
    


