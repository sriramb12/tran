#!/run/pkg/TWW-python-/2.7.2/bin/python
# -*- coding: utf-8 -*-

import os
import sys

## GENERATE TRANSCEND STATIC WEBSITE 
print "Setup globals"
BUILD_DIR     = "./docs/BUILD/"
TOP_HTML      = "./docs/top.html"
BOTTOM_HTML   = "./docs/bottom.html"
FAQS_HTML     = "./docs/faq.html"
INDEX_HTML    = "./docs/index.html"
TUTORIAL_HTML = "./docs/tutorial.html"
ARCHIVE_HTML  = "./docs/release_archive.html"
MANPAGE_HTML  = "./docs/trans.html"
RCPAGE_HTML   = "./docs/transrc.html"

print "Create destination dir"
if not os.path.exists(BUILD_DIR):
    os.mkdir(BUILD_DIR)

print "Reading common parts"
top     = open(TOP_HTML, 'r').read()
bottom  = open(BOTTOM_HTML, 'r').read()

print "Doing FAQ"
iFaq = open(FAQS_HTML, 'r').read()
oFaq = open(BUILD_DIR + "faq.html", 'w')
print >>oFaq, top
print >>oFaq, iFaq
print >>oFaq, bottom
oFaq.close()

print "Doing index"
iIndex = open(INDEX_HTML, 'r').read()
oIndex = open(BUILD_DIR + "index.html", 'w')
print >>oIndex, top
print >>oIndex, iIndex
print >>oIndex, bottom
oIndex.close()

print "Doing Tutorial"
iTutorial = open(TUTORIAL_HTML, 'r').read()
oTutorial = open(BUILD_DIR + "tutorial.html", 'w')
print >>oTutorial, top
print >>oTutorial, iTutorial
print >>oTutorial, bottom
oTutorial.close()

print "Doing Release Archive"
iArchive = open(ARCHIVE_HTML, 'r').read()
oArchive = open(BUILD_DIR + "release_archive.html", 'w')
print >>oArchive, top
print >>oArchive, iArchive
print >>oArchive, bottom
oArchive.close()

print "Doing Transcend Manpage"
iManpage = open(MANPAGE_HTML, 'r').read()
oManpage = open(BUILD_DIR + "trans.html", 'w')
print >>oManpage, top
print >>oManpage, "<frame>"
print >>oManpage, iManpage
print >>oManpage, "</frame>"
print >>oManpage, bottom
oManpage.close()

print "Doing .transrc Manpage"
iManpage2 = open(RCPAGE_HTML, 'r').read()
oManpage2 = open(BUILD_DIR + "transrc.html", 'w')
print >>oManpage2, top
print >>oManpage2, "<frame>"
print >>oManpage2, iManpage2
print >>oManpage2, "</frame>"
print >>oManpage2, bottom
oManpage2.close()

print "done."
sys.exit(0)
