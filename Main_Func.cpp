#include <thread>
#include <unistd.h>
#include <unordered_map>
#include <string.h>
#include <hiredis/hiredis.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <map>
#include "xtp_trader_api.h"
#include "trade_spi.h"
#include "FileUtils.h"
#include "xtp_quote_api.h"
#include "quote_spi.h"

using namespace std;

unordered_map<string, string> md_config;
XTP::API::TraderApi* pUserApi;
bool is_connected_ = false;
std::string trade_server_ip;
int trade_server_port;
uint64_t session_id_ = 0;
std::map<uint64_t,uint64_t> map_session;
uint64_t* session_arrary = NULL;
FileUtils* fileUtils = NULL;
XTPOrderInsertInfo* orderList = NULL;
std::string quote_server_ip;
int quote_server_port;
std::string quote_username;
std::string quote_password;
XTP_PROTOCOL_TYPE quote_protocol = XTP_PROTOCOL_UDP;

string erase_num(string data)
{
	string::iterator it = data.begin();
    while (it != data.end()) {
        if ((*it >= '0') && (*it <= '9')) {
            it = data.erase(it);
        } else { // 必须加else
            it++;
        }
    }
	return data;
}

void read_config(string file_name)
{
	int cfg_line = 0;
	//定义缓存字符串变量
	string st;
	string key, value;
	//定义文件变量
	ifstream infile;
	string::size_type idx;
	char* p = NULL, * q = NULL;
	//打开文件
	infile.open(file_name);

	//遍历直到文件的最后
	while (!infile.eof())
	{
		//初始化缓存
		st = "";
		//取得一行配置
		infile >> st;
		//找不到等号则继续
		idx = st.find(",");
		if (idx == string::npos)
		{
			continue;
		}
		//截断字符串得到key和value字符串
		key = st.substr(0, idx);
		value = st.substr(idx + 1, st.length() - idx);
		md_config[key] = value;
		//插入链表
	}
	//关闭文件
	infile.close();
}

void market_subscribe(char* *allInstruments_sz, int sz_num, char* *allInstruments_sh, int sh_num)
{	
	FileUtils* fileUtils = new FileUtils();
	if (!fileUtils->init())
	{
		std::cout << "The config.json file parse error." << std::endl;
#ifdef _WIN32
		system("pause");
#endif

		return;
	}
	int client_id = fileUtils->intForKey("client_id");
	std::string filepath = fileUtils->stdStringForKey("path_linux");
	quote_server_ip = fileUtils->stdStringForKey("quote_ip");
	quote_server_port = fileUtils->intForKey("quote_port");
	quote_username = fileUtils->stdStringForKey("quote_user");
	quote_password = fileUtils->stdStringForKey("quote_password");
	quote_protocol = (XTP_PROTOCOL_TYPE)(fileUtils->intForKey("quote_protocol"));
	int32_t quote_buffer_size = fileUtils->intForKey("quote_buffer_size");
	int32_t heat_beat_interval = fileUtils->intForKey("hb_interval");
	XTP::API::QuoteApi* pQuoteApi = XTP::API::QuoteApi::CreateQuoteApi(client_id, filepath.c_str(), XTP_LOG_LEVEL_DEBUG);
	MyQuoteSpi* pQuoteSpi = new MyQuoteSpi();
	pQuoteApi->RegisterSpi(pQuoteSpi);
	pQuoteApi->SetHeartBeatInterval(heat_beat_interval); 
	pQuoteApi->SetUDPBufferSize(quote_buffer_size);
	int loginResult_quote = -1;
	loginResult_quote = pQuoteApi->Login(quote_server_ip.c_str(), quote_server_port, quote_username.c_str(), quote_password.c_str(), quote_protocol); 
	cout<<"loginResult_quote="<<loginResult_quote<<endl;
	if (loginResult_quote == 0)
	{
		pQuoteApi->SubscribeMarketData(allInstruments_sz, sz_num, (XTP_EXCHANGE_TYPE)2);
		pQuoteApi->SubscribeMarketData(allInstruments_sh, sh_num, (XTP_EXCHANGE_TYPE)1);
	}
}

int main()
{
	read_config("/root/other/xtp/hs300.txt");
	redisContext* my_redis = redisConnect("127.0.0.1", 6379);
	int pair_num = 300;
	int sh_num = 0;
	int sz_num = 0;
	unordered_map<string, string>::iterator it;
	char* *allInstruments_sh = new char*[185];
	char* *allInstruments_sz = new char*[115];
	for (int i = 0; i < 185; i++) {
		allInstruments_sh[i] = new char[7];
		string instrument ="000000";
		strcpy(allInstruments_sh[i], instrument.c_str());
	}
	for (int i = 0; i < 115; i++) {
		allInstruments_sz[i] = new char[7];
		string instrument ="000000";
		strcpy(allInstruments_sz[i], instrument.c_str());
	}
	for(unordered_map<string, string>::iterator iter=md_config.begin();iter!=md_config.end();iter++)
	{
		string instrument = iter->first;
		string exchange = iter->second;
		// cout<<instrument<<", "<<exchange<<endl;
		if(exchange=="sh")
		{
			strcpy(allInstruments_sh[sh_num], instrument.c_str());
			sh_num++;
		}
		else if(exchange=="sz")
		{
			strcpy(allInstruments_sz[sz_num], instrument.c_str());
			sz_num++;
		}
	}
	cout<<"sh_num="<<sh_num<<endl;
	cout<<"sh_num="<<sz_num<<endl;

	market_subscribe(allInstruments_sz, sz_num, allInstruments_sh, sh_num);

	// for(int i=0;i<pair_num;i++)
	// {
	// 	contract_input data_list;
	// 	data_list.instrumentid = md_config["contract"+std::to_string(i+1)];
	// 	// data_list.exchangeid = md_config["exchangeid"+std::to_string(i+1)];
	// 	data_list.exchangeid = "SHFE";
	// 	cout<<data_list.instrumentid<<", "<<data_list.exchangeid <<endl;
	// 	contract_list.push_back(data_list);
	// 	string instrumentid=data_list.instrumentid;
	// 	instrumentid = erase_num(instrumentid);
	// 	cout<<instrumentid<<endl;
	// 	string command1 = "set "+instrumentid+"_main "+ data_list.instrumentid; 
	// 	(redisReply*)redisCommand(my_redis, command1.c_str()); 

	// }
	// md_run(contract_list, pair_num);
	

    while(1) {}
	return 0;
}
