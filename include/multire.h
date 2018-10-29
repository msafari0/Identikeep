// #include "tinyregex/re.h"
#include <vector>
#include <string>
#include <sstream>
#include <regex.h>        


// Logging
#include "plog/Log.h"

/* This function returns all matches to a regex that has only ONE 
 * capturing group, example:
 *      SomeValue[\s\t]+:(\d+)
 *                       ^---^ capturing group
 *
 * All results are store in a vector of string, if no matching is found 
 * for the capturing group, the list is empty.
 */



static std::vector<std::string> DoRegexMatch(const std::string& text, const std::string& expr)
{
    
    /* Hosts results, all of type string since we do string matching here*/ 
    std::vector<std::string> results;
    std::stringstream lines(text);

    regex_t regex;
    int err;
    err=regcomp(&regex, expr.c_str(), REG_EXTENDED);
    if(err){
        results.push_back("Cannot compile");
        return results;
    }
    regmatch_t pmatch[2];
    while(!lines.eof()){
        std::string line;
        std::getline(lines, line, '\n' );
        err = regexec(&regex, line.c_str(), 2, pmatch, 0);
        if(!err) results.push_back(line.substr(pmatch[1].rm_so, pmatch[1].rm_eo-pmatch[1].rm_so));
    }
    
    return results;
}

