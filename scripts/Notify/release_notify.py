#!/run/pkg/TWW-python-/2.7.2/bin/python
# -*- coding: utf-8 -*-

## SEND HTML EMAILS
import re
import os
import smtplib
import imaplib, email
from datetime import datetime
from email.mime.text import MIMEText
from email.mime.multipart import MIMEMultipart


def MAIN():
    log = open("email_sent.log", "a")
    print("MAIN")
    tolist  = list_coreids("transweb")
    #tolist  = ['transbug@nxp.com;100']
    famedc  = "FAMEDC@nxp.com"
    msg     = MIMEMultipart('alternative')
    body    = MIMEText(html_body('transweb', 'release'), 'html')
    ret     = None

    msg.attach(body)

    ## Send the email via our own SMTP server
    s = smtplib.SMTP("remotesmtp.freescale.net")
    #s.set_debuglevel(1) # enable debug output

    msg['From']     = famedc
    msg['To']       = famedc
    msg['Subject']  = build_subject('transweb', 'release')

    for email in tolist:
        print("email:", email)
        log.write('current line is: {0} where the email is: {1}\n'.format(email, email.split(';')[0]))
        msg['Bcc'] = email.split(';')[0]
        ret = s.sendmail(famedc, email, msg.as_string())
        if ret:
            failed_addresses = ret.keys()
            log.write('recipient refused: {0}\n'.format(failed_addresses))
    s.quit()
    print("BYE")


def list_coreids(tool):
    if os.path.isfile('db_final.txt'):
        with open('db_final.txt') as f:
            cache = f.readlines()
        return [s.strip() for s in cache]
    else:
        raise "error no db_final.txt found"
        sys.exit(666)


def html_body(tool, reason):
    if reason == 'release':
        return open('./TEMPLATES/transcend_transweb_release_notes.html').read()
    else:
        if tool == 'transcend':
            return open('./TEMPLATES/transcend_scheduled_outage.html').read()
        else:
            raise "no html template set"
            sys.exit(666)


def build_subject(tool, reason):
    if reason == 'release':
        return "Announcement: Transcend 9.3.1 and TransWeb 3.5.3"
    else:
        return "Transcend/Intercom/Transweb scheduled maintenance outage for November 7th starting at 6AM Phoenix time"


if __name__ == "__main__": MAIN()

