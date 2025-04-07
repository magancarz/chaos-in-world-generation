#!/bin/bash

MAX_ARGUMENTS=4
MAX_LINES_OF_CODE=60
MAX_COMPLEXITY=4
SOURCES="Source/ Include/"

lizard \
	--verbose --warnings_only --languages cpp \
	--arguments $MAX_ARGUMENTS \
	--Threshold nloc=$MAX_LINES_OF_CODE \
	--CCN $MAX_COMPLEXITY \
	--working_threads $(nproc --all) \
	$SOURCES
