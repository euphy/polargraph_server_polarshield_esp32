#!/usr/bin/python3

import subprocess
from datetime import datetime 

version = subprocess.Popen("git describe", shell=True, stdout=subprocess.PIPE).stdout.read().decode()
git_status = subprocess.Popen("git status -s -uno", shell=True, stdout=subprocess.PIPE).stdout.read().decode()




if git_status:
    local_changes = "+{:d}ch".format(git_status.count('\n'))
    output = version.strip() + "@{:s}".format(datetime.now().strftime("%Y%m%d.%H%M%S")) + local_changes
else:
    output = version.strip()


print('String firmwareBuildName="{:s}";'.format(output))

