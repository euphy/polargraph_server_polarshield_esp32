#!/usr/bin/python
import subprocess

set_index_for_version_file = subprocess.Popen("git update-index --assume-unchanged .\\src\\firmware-build-name.h")