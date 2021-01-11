#!/usr/bin/python3

import smtplib

server_host = '127.0.0.1'
server_port = 8080

def run_test(test, test_name):
    try:
        test()
        print(test_name + " OK")
    except Exception as e:
        print(test_name + " Failed")
        print(e)

def simple_mail_test():
    from_addr = "sych@test.ru"
    to_addr = "alex@local.com"

    data = """X-From: {0}\n X-Original-To: {1}\n Some text for mail\n With many lines\n""".format(from_addr, to_addr)

    server = smtplib.SMTP(server_host, server_port)
    server.sendmail(from_addr, to_addr, data)
    server.quit()

print("start SMTP server system tests...")
run_test(simple_mail_test, "simple_mail_test")
print("finish SMTP server system tests.")
