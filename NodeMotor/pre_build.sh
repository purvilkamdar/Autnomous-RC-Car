#!/bin/bash

# 
# This file executes from the build directory by the Eclipse "Pre Build" step
#

python ../_can_dbc/dbc_parse.py -i C:/SJSU_Dev_CmpE_243/projects/TITANS_PROJECT_RC_CAR/243.dbc -s MOTOR > ../_can_dbc/generated_can.h