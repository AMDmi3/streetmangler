#!/bin/sh -u

datadir="`dirname $0`/../data"

automaint_files="
	$datadir/ru/areas.txt
	$datadir/ru/bridges.txt
	$datadir/ru/exceptions.txt
	$datadir/ru/fixme.txt
	$datadir/ru/main.txt
	$datadir/ru/multistatus.txt
	$datadir/ru/nested.txt
	$datadir/ru/unofficial.txt
	$datadir/ru_BY.txt
	$datadir/ru_RU.txt
	"

all_files="
	$automaint_files
	$datadir/ru/ambiguities.txt
	"

area_statuses="посёлок|район|микрорайон|квартал|деревня"
nonarea_statuses="аллея|переулок|площадь|проезд|проспект|прощадь|тракт|улица|шоссе|линия|тупик|набережная|бульвар"
