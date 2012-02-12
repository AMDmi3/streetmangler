#!/bin/sh -u

datadir="`dirname $0`/../data"

automaint_files="
	$datadir/ru/areas.txt
	$datadir/ru/exceptions.txt
	$datadir/ru/main.txt
	$datadir/ru_BY.txt
	$datadir/ru_RU.txt
	"

all_files="
	$automaint_files
	$datadir/ru/ambiguities.txt
	"
