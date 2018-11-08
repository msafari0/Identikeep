#!/usr/bin/python

import json
import sys
       
        

def parse_data(data):
    global_list=[]
    for node in data["NodeList"]:
        for proc in node["ProcList"]:
            for key in proc.keys():
                if key!="GlobalRank":
                    for plugkey in proc[key]:
                        unit=[]
                        try:
                            unit=proc[key][plugkey]["unit"]
                        except:
                            unit=""
                        global_list.append([node["NodeName"],proc["GlobalRank"] , key , plugkey, proc[key][plugkey]["value"], unit])

    return global_list    


def print_nodelist(global_list):
    lastnode=""
    lastproc=""
    node_list=[]
    rank_list=[]


    print "Nodes List:"
    for value in global_list:
        if value[0]!=lastnode:
            lastnode=value[0]
            lastproc=value[1]
            node_list.append(value[0])
            rank_list.append([value[1]])
        else:
            if value[1]!=lastproc:
                lastproc=value[1]
                rank_list[-1].append(value[1])
                
    for i in range(0,len(node_list)):
        outstring="\t"+node_list[i]+"\t\t"
        outstring+=str(rank_list[i][0])
        if len(rank_list[i])==2:
            outstring+=","+str(rank_list[i][1])
        if len(rank_list[i])>2:
            outstring+="->"+str(rank_list[i][-1])
        print outstring
    

def print_pluginlist(global_list):

    plugin_list=[]
    field_list=[]
    outputlist=[]
    print "Plugins List:"
    for value in global_list:
        if value[2] not in plugin_list:
            field_list.append([value[3]])
            plugin_list.append(value[2])
        else:
            index=plugin_list.index(value[2])
            if value[3] not in field_list[index]:
                field_list[index].append(value[3])            

                
            #lastproc=value[1]
            
    for index in range(0,len(plugin_list)):
        print "\t" + plugin_list[index]
        for key in field_list[index]:
            print "\t\t" + key
         

def print_field(global_list, field_list):
    for field in field_list:
        found=0
        print "\n"+field
        for value in global_list:
            if value[3]==field:
                found=1
                outstring="\tRank "+str(value[1])+" on "+value[0]+":\t"
                if len(value[4])==1:
                    outstring+=str(value[4][0])+" "+value[5]
                else:
                    for val in value[4]:
                        outstring+="\n\t\t\t"+str(val)+" "+value[5]
                print outstring

        if not found:
            print "\tNo entries found!"
        
        
def match_field(global_list, field_name):
    field_list=[]
    for value in global_list:
        if value[3].find(field_name)>=0:
            if value[3] not in field_list:
                field_list.append(value[3])
    
    return field_list
            

def filter_node(global_list, nodename):
    filtered_list=[]
    for i in range(0, len(global_list)):
        if global_list[i][0]==nodename:
            filtered_list.append(global_list[i])
    return filtered_list

def filter_rank(global_list, procrank):
    filtered_list=[]
    for i in range(0, len(global_list)):
        if int(global_list[i][1])==int(procrank):
            filtered_list.append(global_list[i])
    return filtered_list



def print_info(data):
    print "Date: \t\t\t" + data["DateTime"]
    print "Execution Time: \t" + str(data["ExecTime"])+"s"
    if len(data["Comment"])>0:
        print "Comment:"
        print "\t"+data["Comment"]
        
        
        
def print_help():
    print "Usage: identiview  FILENAME [OPTIONS]\n"
    print "Mandatory arguments:"
    print "%24s" % "FILENAME" + "\tfile containing the identikeep output in .json format"
    print "Options:"
    print "%24s" % "-h, --help" + "\tprint this help"
    print "%24s" % "-ln, --list-nodes" + "\tlist all node names"
    print "%24s" % "-lf, --list-fields" + "\tlist all available fields in the .json file"
    print ""
    print "%24s" % "-n, --node NODENAME" + "\tRestrict the analysis to node NODENAME"
    print "%24s" % "-r, --rank RANK" + "\tRestrict the analysis to rank RANK"
    print "%24s" % "-f, --field FIELDNAME" + "\tPrint only fields containing the string FIELDNAME in their names"

    
    

########################################################################################

def main():
    for option in sys.argv:
        if option=="-h" or option=="--help":
            print_help()
            return
            
    if len(sys.argv)<2:
        print "File name required. Type --help for more info"
        sys.exit(1)

    print "Opening file " + sys.argv[1]
    json_data=[]
    data=[]
    try:
        json_data=open(sys.argv[1]).read()
    except:
        print "Error while reading file "+sys.argv[1]+ " . Aborting."
        sys.exit(1)
    
    try:
        data = json.loads(json_data)
    except:
        print sys.argv[1] + " seems to be not properly formatted. Aborting"
        sys.exit(1)

    global_list=parse_data(data)
    #print global_list
    
    option=sys.argv
    field_list=match_field(global_list, "")
    i=2
    while i<len(option):
        if option[i]=="-i" or option[i]=="--info":
            print_info(data)
            return
        elif option[i]=="-ln" or option[i]=="--list-nodes":
            print_nodelist(global_list)
            return
        elif option[i]=="-lf" or option[i]=="--list-fields":
            print_pluginlist(global_list)
            return     
        elif (option[i]=="-f" or option[i]=="--field") and i+1<len(option):
            field_list=match_field(global_list, option[i+1])
            i=i+1
        elif (option[i]=="-n" or option[i]=="--node")  and i+1<len(option):
            print "Analysis retricted to node "+option[i+1]
            global_list=filter_node(global_list,option[i+1] )
            if len(global_list)==0:
                print "Unable to find node " + option[i+1] + " in file " + sys.argv[1] +". Aborting."
                return
            i=i+1

        elif (option[i]=="-r" or option[i]=="--rank")  and i+1<len(option):
            print "Analysis retricted to rank "+option[i+1]
            global_list=filter_rank(global_list,option[i+1] )
            if len(global_list)==0:
                print "Unable to find rank " + str(option[i+1])+ " in file " + sys.argv[1] +". Aborting."
                return
            i=i+1
        else:
            print "Error. Option "+option[i]+" not recognized."
            return
        i+=1

    print_field(global_list, field_list)
    return  
        




if __name__ == "__main__":
    main()
