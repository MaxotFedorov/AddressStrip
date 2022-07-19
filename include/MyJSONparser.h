#include <Arduino.h>

int MyJSONparser_start(String json);
String MyJSONparser_get_value(String key);
void MyJSONparser_set_value(String key, String value);
String MyJSONparser_jsonToStr();