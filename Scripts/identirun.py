#!@PYTHON_EXECUTABLE@
import sys
import os 
import platform
import subprocess
import datetime



def check_prog(name):
    cmd = "type"
    with open(os.devnull, 'wb') as devnull:
        return subprocess.call(cmd + " " + name,stdout=devnull,stderr=devnull, shell=True)


path="@EXECUTABLE_OUTPUT_PATH@"    

args=sys.argv[:]
args.pop(0)
exec_candidate=""

for name in args:
    if(check_prog(name)==0):
        exec_candidate = name

if exec_candidate == "":
    print "Error: no executable was provided. Aborting"
    exit (1)

exec_args = []
mpi_args = []

exec_index=args.index(exec_candidate)

for i in range(0,exec_index):
    mpi_args.append(args[i])

if len(args) > exec_index+1:
    for i in range(exec_index+1, len(args)):
        exec_args.append(args[i])


identikeep_command="mpirun"
mpi_command="mpirun"
for a in mpi_args:
    identikeep_command+= " " + a
    mpi_command+=" " + a


now = datetime.datetime.now()
exec_date=now.strftime("%Y%m%d_%H%M%S")
    
exec_clean=os.path.basename(exec_candidate)
comment="'Automatically created by identirun for executable " + exec_candidate + "'"    
    
identikeep_command+=" " + path+"identikeep " + " -p="+path + " -c="+comment + " -t="+exec_clean+"_"+exec_date
mpi_command+=" " + exec_candidate
for a in exec_args:
    mpi_command+=" " + a

print "Running identikeep with command '" + identikeep_command + "'"
os.system(identikeep_command)

print "Running " + exec_candidate + " with command '" + mpi_command + "'"
os.system(mpi_command)
