#!/usr/bin/python
# -*- coding: utf-8 -*-
# call using
# > py.test -s -k test_HTML_generator.py

# external imports
import re
import traceback

# internal imports
from htmllib import HTML, HTMLGenerator
import tools


def test_HTMLGenerator():

    html = HTMLGenerator()
    html.plug()

    with html.plugged():
        html.open_div()
        text = html.drain()
        assert text.rstrip('\n').rstrip(' ') == "<div>"

    with html.plugged():
        #html.open_div().write("test").close_div()
        html.open_div()
        html.write("test")
        html.close_div()
        assert tools.compare_html(html.drain(), "<div>test</div>")

    with html.plugged():
        #html.open_table().open_tr().td("1").td("2").close_tr().close_table()
        html.open_table()
        html.open_tr()
        html.td("1")
        html.td("2")
        html.close_tr()
        html.close_table()
        assert tools.compare_html(html.drain(), "<table><tr><td>1</td><td>2</td></tr></table>")

    with html.plugged():
        html.div("test", **{"</div>malicious_code<div>": "trends"})
        assert tools.compare_html(html.drain(), "<div &lt;/div&gt;malicious_code&lt;div&gt;=trends>test</div>")

    a = u"\u2665"
    with html.plugged():
        assert html.render_a("test", href="www.test.case")
        html.render_a(u"test", href="www.test.case")
        html.render_a("test", href=u"www.test.case")
        html.render_a(u"test", href=u"www.test.case")
        try:
            assert html.render_a(u"test", href=unicode("www.test.case"), id_=unicode("something"), class_=unicode("test_%s") % a)
        except Exception, e:
            print traceback.print_exc()
            print e


def test_multiclass_call():
    html = HTMLGenerator()
    html.plug()

    with html.plugged():
        html.div('', class_="1", css="3", cssclass = "4", **{"class": "2"})
        assert tools.compare_html(html.drain(), "<div class=\"1 3 4 2\"></div>")


def test_exception_handling():
    html = HTMLGenerator()
    try:
        raise Exception("Test")
    except Exception, e:
        assert tools.compare_html(html.render_div(e), "<div>%s</div>" % e)
