#!/usr/bin/env python
# -*- encoding: utf-8 -*-
# vim: sts=4 sw=4 et

#from nose.tools import *

from streetmangler import Locale, Name, Database

locale = Locale('ru_RU')

street = u'улица Ленина'
street_list = [u'улица Ленина', u'Ленина улица', u'ул. Ленина']

def test_exact():
    db = Database(locale)
    assert(not db.CheckExactMatch(street))
    db.Add(street)
    assert(db.CheckExactMatch(street))
    assert(db.CheckExactMatch(Name(street, locale)))
    assert(not db.CheckExactMatch('Ленина улица'))
    assert(not db.CheckExactMatch('ул Ленина'))

def test_canonical():
    db = Database(locale)
    assert(db.CheckCanonicalForm(street) == [])
    db.Add(street)
    for n in street_list:
        assert(db.CheckCanonicalForm(n) == [street])
        assert(db.CheckCanonicalForm(Name(n, locale)) == [street])

def test_spelling():
    street = u'улица Лемина'
    db = Database(locale)
    assert(db.CheckSpelling('улица Ленинa') == [])
    db.Add(street)
    for n in [street] + street_list:
        assert(db.CheckSpelling(n) == [street])
        assert(db.CheckSpelling(Name(n, locale)) == [street])
