#include "MyJSONparser.h"

String keys[10];
String values[10];
int num_of_pairs = 0;
//String str = "{\"led_state\" : false, \"brightness\" : 16, \"color\" : \"#ff0000\"}";

int MyJSONparser_start(String json)
{
    int i = 0;
    for(int k = 0;;k++)
    {
        while(json[i] != '\"')
            i++;
        i++;
        while(json[i] != '\"'){
            keys[k] += json[i];
            i++;
        }
        i++;
        while(json[i] != ':')
            i++;
        i++;
        while(json[i] == ' ')
            i++;
        while(json[i] != ','){
            values[k] += json[i];
            i++;
            if(json[i] == '}')
            {
                num_of_pairs++;
                return k;
            }
        }
        i++;
        num_of_pairs++;
    }
    return 0;
}

String MyJSONparser_get_value(String key){
    for(int i = 0; i < num_of_pairs; i++)
    {
        if(keys[i].equals(key))
            return values[i];
    }
    return "none";
}

void MyJSONparser_set_value(String key, String value){
    for(int i = 0; i < num_of_pairs; i++)
    {
        if(keys[i].equals(key))
            values[i] = value;
    }
}

String MyJSONparser_jsonToStr(){
    String strjson = "";
    strjson += '{';
    for(int i = 0; i < num_of_pairs; i++){
        strjson += "\"" + keys[i] + "\"" + " : " + values[i];
        if(num_of_pairs - i > 1)
            strjson += ", ";
    }
    strjson += '}';
    return strjson;
}