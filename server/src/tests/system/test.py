#!/usr/bin/python3

import sys
import os
import shutil
import smtplib

server_host = '127.0.0.1'
server_port = 8080
local_domain = "local.com"
base_maildir = "/home/netroot/test_mail"

if (len(sys.argv) != 2):
    print("usage: test.py [test_data_dir]")
    exit(1)

test_data_dir = sys.argv[1]


def get_mail_filenames(to_addr):
    dir = base_maildir
    if (to_addr.find("@" + local_domain) != -1):
        name_end = to_addr.find("@")
        name = to_addr[0:name_end]
        dir += "/" + name
    dir += "/maildir/new"
    return list(map(lambda x: dir + "/" + x, os.listdir(dir)))


def check_mail(filename, data, from_addr, to_addrs, print_errors = True):
    f = open(filename)
    lines = f.readlines()
    f.close()
    if lines[0] != "X-From: " + from_addr + "\n":
        if (print_errors):
            print("Error " + from_addr)
        return False
    i = 0
    for to_addr in to_addrs:
        i += 1
        if lines[i] != "X-Original-To: " + to_addr + "\n":
            if (print_errors):
                print("Error " + to_addr)
            return False

    i += 1 # пропускаем \n
    data_lines = data.split("\n")
    for data_line in data_lines:
        i += 1
        data_line += "\n"
        if lines[i] != data_line:
            if (print_errors):
                print("lines[i] != data_line : " + lines[i] + " | " + data_line + "| i: " + str(i))
            return False

    i += 1
    return lines[i] == ".\n"


def run_test(test, test_name):
    res = False
    try:
        res = test()
    except Exception as e:
        res = False
        print(e)

    if (res):
        print(test_name + " OK")
    else:
        print(test_name + " Failed")


def simple_mail_test():
    # Arrange
    from_addr = "sych@test.ru"
    to_addr = "simple_mail@local.com"

    data = "Text for mail\nWith many lines..."

    # Act
    server = smtplib.SMTP(server_host, server_port)
    server.sendmail(from_addr, to_addr, data)
    server.quit()

    # Assert
    filenames = get_mail_filenames(to_addr)
    if (len(filenames) != 1):
        return False
    return check_mail(filenames[0], data, from_addr, [ to_addr ])


def many_mails_test():
    # Arrange
    from_addr = "sych@test.ru"
    to_addr = "many_mails@local.com"

    n = 5
    datas = ["Letter {0}\nWith many lines...".format(i) for i in range(n)]

    # Act
    server = smtplib.SMTP(server_host, server_port)
    for data in datas:
        server.sendmail(from_addr, to_addr, data)
    server.quit()

    # Assert
    filenames = get_mail_filenames(to_addr)
    if (len(filenames) != n):
        return False

    for data in datas:
        res = False
        for filename in filenames:
            res = res or check_mail(filename, data, from_addr, [ to_addr ], False)
        if(not res):
            print("Can`t find mail for data: " + data)
            return False

    return True


def one_mb_mail_test():
    # Arrange
    from_addr = "sych@test.ru"
    to_addr = "one_mb_mail@local.com"
    f = open(test_data_dir + "/one_mb.txt")
    data = f.read()
    f.close()

    # Act
    server = smtplib.SMTP(server_host, server_port)
    server.sendmail(from_addr, to_addr, data)
    server.quit()

    # Assert
    filenames = get_mail_filenames(to_addr)
    if (len(filenames) != 1):
        return False
    return check_mail(filenames[0], data, from_addr, [ to_addr ])


def many_rcpt_mail_test():
    # Arrange
    from_addr = "sych@test.ru"
    to_addrs = ["many_rcpt1@remote.com", "many_rcpt2@remote.com", "many_rcpt3@local.com", "many_rcpt4@local.com"]
    data = "Mail for many rcpts\nAnd many lines..."

    # Act
    server = smtplib.SMTP(server_host, server_port)
    server.sendmail(from_addr, to_addrs, data)
    server.quit()

    # Assert
    for to_addr in to_addrs:
        filenames = get_mail_filenames(to_addr)
        if (len(filenames) != 1):
            return False
        return check_mail(filenames[0], data, from_addr, to_addrs)

    return True


print("start SMTP server system tests...")
shutil.rmtree(base_maildir, ignore_errors=True)
print("base maildir removed")
run_test(simple_mail_test, "simple_mail_test")
run_test(many_mails_test, "many_mails_test")
run_test(one_mb_mail_test, "one_mb_mail_test")
run_test(many_rcpt_mail_test, "many_rcpt_mail_test")
print("finish SMTP server system tests.")
