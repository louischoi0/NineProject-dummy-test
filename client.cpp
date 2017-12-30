#include "stdafx.h"
#include "net.h"

int client_no_ssl::index = 0;

const std::string testid = "test_id";
const std::string testpw = "test_pw";

void BormNet::accept_command()
{	
	while (1)
	{
		std::cout << " waiting for command " << std::endl;

		std::cout << "Command > " << std::endl;

		std::string res;
		std::getline(std::cin, res);

		boost::split(stringCont_, res, boost::is_any_of(" "));

		route_action();
	}

}

void create_instance(int i, std::vector<client_no_ssl*>& C, std::vector<boost::asio::io_service::strand>& S ,boost::asio::io_service* io)
{
	boost::asio::ip::tcp::resolver resolver(*io);
	boost::asio::ip::tcp::resolver::iterator iterator = resolver.resolve(Query);
	
	for (int k = 0; k < i; k++)
	{
		client_no_ssl* client = new client_no_ssl(*io, iterator);
		C.push_back(client);
		S.push_back(boost::asio::io_service::strand(*io));
		
		io->post( S[k].wrap( boost::bind(&client_no_ssl::start, C[k] , iterator)));
	}

}


void log_instance(int i, std::vector<client_no_ssl*>& C, std::vector<boost::asio::io_service::strand>& S, boost::asio::io_service* io)
{
	boost::asio::ip::tcp::resolver resolver(*io);
	boost::asio::ip::tcp::resolver::iterator iterator = resolver.resolve(Query);
	
	for (int k = 0; k < i; k++)
	{
		std::string id_for_instance = testid + std::to_string(k);
		std::string pw_for_instance = testpw + std::to_string(k);

		id_for_instance = boost::locale::conv::to_utf<char>(id_for_instance, "EUC-KR");
		pw_for_instance = boost::locale::conv::to_utf<char>(pw_for_instance, "EUC-KR");

		io->post(S[k].wrap(boost::bind(&client_no_ssl::req_login, C[k])));
	}

}

void init_users(int i , std::vector<client_no_ssl*>& C, std::vector<boost::asio::io_service::strand>& S, boost::asio::io_service* io )
{

	create_instance(i , C , S, io);
	log_instance(i, C, S ,io);

}
void BormNet::view_chat_list()
{
	std::for_each(roomAssign_.begin(), roomAssign_.end(), [](std::list<std::string>& chat)
	{
		std::cout << std::endl;
		auto iter = chat.begin();
		std::for_each(chat.begin(), chat.end(), [](std::string& clientName)
		{
			std::cout << clientName << std::endl;
		});
	});
}
bool BormNet::route_set_actions(std::vector <std::string>& cont)
{
	auto iter = cont.begin();
	if (*iter == "set")
	{
		if (*(iter + 1) == "connections")

		{
			int min = stoi(*(iter + 2));
			int max = stoi(*(iter + 3));

			assign_connection(min, max);
		}

		else if (*(iter + 1) == "tables")
		{
			int count = stoi(*(iter + 2));
			int countPerSession = stoi(*(iter + 3));
			assign_chat_room(count, countPerSession);
		}
		return true;
		

	}
	
	return false;
}

bool BormNet::route_get_actions(std::vector<std::string>& cont)
{
	auto iter = cont.begin();
	if (*iter == "get")
	{
		if (*(iter + 2) == "flist")
		{
			int index = stoi(*(iter + 1));
			userPool_[index].activate_with_strand_no_err(boost::bind(&client_no_ssl::get_friend_list, &userPool_[index]));
		}

		return true;
	}

	return false;
}

bool BormNet::route_view_actions(std::vector<std::string>& cont )
{
	auto iter = cont.begin();

	if (*iter == "view")
	{
		if (*(iter + 1) == "connections")
			view_connection();

		else if (*(iter + 1) == "tables")
			view_chat_list();

		return true;
	}

	return false;
}

void BormNet::route_action()
{
	auto it = stringCont_.begin();
	boost::asio::ip::tcp::resolver resolver(clientService_);
	
	std::vector<client_no_ssl>& C = userPool_;

	if (route_set_actions(stringCont_))
		return;
	else if (route_view_actions(stringCont_))
		return;
	else if (route_get_actions(stringCont_))
		return;

	int index = 0;
	if (*it == "system")
	{
		if (*(it + 1) == "init")
		{
			int capacity = stoi(*(it + 2));
			system_init(capacity);
		}

		else if (*(it + 1) == "run")
		{
			run_net();
		}
	}

	if (*it == "get")
	{
		if (*(it + 1) == "msg")
		{
			int index = stoi(*(it + 2));
//			C[index].get_msg_req();
		}

		else if (*(it + 1) == "ivt")
		{
			int index = stoi(*(it + 2));
			C[index].get_ivt_req();
					
		}

		else if (*(it + 1) == "conc")
		{
			int index = stoi(*(it + 2));
			C[index].get_conc_req();
			
		}
	}


	else if (*it == "req")
	{

		if (*(it + 1) == "connection")
		{
			int index = stoi(*(it + 2));
			std::string target = *(it + 3);
			short rank = stoi(*(it + 4));

			C[index].req_connection(target, 0);
		}

		else if (*(it + 1) == "invite")
		{
			int index = stoi(*(it + 2));
			std::string target = *(it + 3);
			short tableKey = stoi(*(it + 4));

			C[index].req_invite(target, tableKey);
		}

		else if (*(it + 2) == "new" || *(it + 3) == "table" )
		{
			int index = stoi( *(it + 1) );
			int size = stoi(*(it + 4) );
			
			std::vector<std::string> lcont;

			for (int i = 0; i < size; i++)
			{
				std::cout << "Invite User " << i << " : " << std::endl;
				
				std::string res;
				std::getline(std::cin, res);
				lcont.push_back(res);

			}
			
			C[index].req_create_table(lcont);
		}
	}

	else if( *it == "connect")
	{
		connect_all_instance();
	}

	else if (*it == "disconnect")
	{
		int index = stoi(*(it + 1));
		C[index].disconnect();
	}

	else if (*it == "send")
	{
		int index = stoi(*(it + 1));
		int tableKey = stoi(*(it + 2));
		
		std::string text = *(it + 3);

		C[index].command_send_msg(tableKey ,0, text);
	}

	else if (*it == "close")
	{
		int index = stoi(*(it + 1));
		C[index].close();

	}

	else if (*it == "sendp")
	{

		int index = stoi(*(it + 1));
		int comd = stoi(*(it + 2));

		std::string s = *(it + 3);

		C[index].send_handle(comd , s.size() , s.c_str());

	}
	

	else if (*it == "log")
	{
		log_in_all_instance();
	}


}