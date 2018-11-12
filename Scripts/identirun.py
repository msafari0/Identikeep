#!@PYTHON_EXECUTABLE@
import sys
import os 
import platform
import subprocess



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
    
exec_clean=exec_candidate.replace("./","")
comment="'Automatically created by identirun for executable " + exec_candidate + "'"    
    
identikeep_command+=" " + path+"identikeep " + " -p="+path + " -c="+comment + " -t="+exec_clean
mpi_command+=" " + exec_candidate
for a in exec_args:
    mpi_command+=" " + a


print identikeep_command
print mpi_command


print "Running identikeep..."
os.system(identikeep_command)

print "Running " + exec_candidate + " ..."
subprocess.call(mpi_command, shell=True)
