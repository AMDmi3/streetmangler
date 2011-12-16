#!/usr/bin/perl
# -*- encoding: utf-8 -*-

use StreetMangler;
use strict;
use utf8;
use Test::Simple;

my $locale = StreetMangler::Locale->new('ru_RU');

my $street = 'улица Ленина';
my $street_list = ('улица Ленина', 'Ленина улица', 'ул. Ленина');

binmode (STDOUT, ":utf-8");
print StreetMangler::Name->new("ул. Ленина", $locale);
print "\n";

sub test_exact {
	my $db = StreetMangler::Database->new($locale);
    ok(!$db->CheckExactMatch($street));
	$db->Add($street);
	ok($db->CheckExactMatch($street));
	ok($db->CheckExactMatch(StreetMangler::Name->new($street, $locale)));
	ok(!$db->CheckExactMatch('Ленина улица'));
	ok(!$db->CheckExactMatch('ул Ленина'));
}

sub test_canonical {
	my $db = StreetMangler::Database->new($locale);
	my $out;
	$out = $db->CheckCanonicalForm($street);
	ok($#$out == -1);
	$db->Add($street);
	foreach my $n ($street_list) {
		$out = $db->CheckCanonicalForm($n);
		ok($#$out == 0 && $out->[0] eq $street);
		$out = $db->CheckCanonicalForm(StreetMangler::Name->new($n, $locale));
		ok($#$out == 0 && $out->[0] eq $street);
	}
}

sub test_spelling {
	my $misspelled = 'улица Лемина';
	my $db = StreetMangler::Database->new($locale);
	my $out;
	$out = $db->CheckSpelling('улица Ленинa');
	ok($#$out == -1);
	$db->Add($street);
	foreach my $n ($misspelled, $street_list) {
		$out = $db->CheckSpelling($n);
		ok($#$out == 0 && $out->[0] eq $street);
		$out = $db->CheckSpelling($n);
		ok($#$out == 0 && $out->[0] eq $street);
	}
}

test_exact();
test_canonical();
test_spelling();

print "All OK\n";
