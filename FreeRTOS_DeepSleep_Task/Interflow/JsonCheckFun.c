#include "JsonCheckFun.h"
#include "stdio.h"

void AddCsToJsonAndPushJsonStr(JsonObject InputJsonStrObj) {
    int CheckNum = 0;
    int AddrOver = InputJsonStrObj.JsonString.getStrlen(&InputJsonStrObj.JsonString);
    // 计算 cs 或其他检验算法
    for (int i = 0; i < AddrOver; i++) {
        CheckNum += InputJsonStrObj.JsonString.Name._char[i];
    }
    InputJsonStrObj.JsonString.Name._char[AddrOver - 1] = '\0';
    AddJsonItemData(InputJsonStrObj.JsonString, ",");
    AddJsonItemData(InputJsonStrObj.JsonString, "NowCheckNum:%d", CheckNum);
    AddJsonItemData(InputJsonStrObj.JsonString, "}");
    return;
}
bool checkOfCsJsonStrIsRight(strnew JsonInputStr, strnew JsonOutputStr) {
    JsonObject JsonObj = newJsonObjectByString(JsonInputStr);
    int NowCheckNum = JsonObj.getInt(&JsonObj, "NowCheckNum");
    char * PEnd = strstr(JsonObj.JsonString.Name._char, ",\"NowCheckNum\"");
    (*(PEnd++)) = '}';
    (*PEnd) = '\0';
    // 计算 cs 或其他检验算法
    int CheckNum = 0;
    for (unsigned int i = 0; i < strlen(JsonInputStr.Name._char); i++) {
        CheckNum += JsonInputStr.Name._char[i];
    }
    if (JsonInputStr.Name._char != JsonOutputStr.Name._char) {
        copyString(JsonOutputStr.Name._char, JsonInputStr.Name._char, JsonOutputStr.MaxLen, JsonInputStr.MaxLen);
    }
    return ((CheckNum == NowCheckNum) ? true : false);
}
