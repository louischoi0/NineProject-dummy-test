#pragma once
#include "stdafx.h"
#include "clt.h"
#include <map>
const boost::asio::ip::tcp::resolver::query Query("188.166.219.190", "8899");
typedef boost::asio::ip::tcp::resolver::iterator endpoint;

// Send msg 1
// Ivtite 8
// Create Table 18
// Connection 20

using namespace boost::asio;
class BormNet
{
	
public:
	BormNet(int capacity, boost::asio::io_service& io, boost::asio::ip::tcp::resolver::iterator endpoint_iterator) :
		capacity_(capacity),
		query_(endpoint_iterator),
		cWork_(clientService_),
		bWork_(bormService_)
	{
		bth = new boost::thread(boost::bind(&io_service::run, &bormService_)); 
	}


	BormNet() :
		capacity_(0),
		cWork_(clientService_),
		bWork_(bormService_)
	{
		query_ = ip::tcp::resolver(clientService_).resolve(Query);
		bth = new boost::thread(boost::bind(&io_service::run, &bormService_));
	
	
	}

	void set_service(int cle , int borm)
	{
		bThreadPool_ = std::vector<boost::thread*>(borm, new boost::thread(boost::bind(&io_service::run, &bormService_)));
		cThreadPool_ = std::vector<boost::thread*>(cle , new boost::thread(boost::bind(&io_service::run, &clientService_)));
	}
	void system_init(int count)
	{
		capacity_ = count;
		set_service( count  , 1 + (count / 5) );
		
		userLoginTryPool_ = std::vector<bool>(count ,false);
		userPool_ = std::vector<client_no_ssl>(count , client_no_ssl(clientService_,query_));
		std::map< std::string, client_no_ssl* >& userIndexing = userIndex_;
		std::for_each(userPool_.begin(), userPool_.end(), [&userIndexing](client_no_ssl& client)->void {
			userIndexing.insert(std::pair<std::string , client_no_ssl*>( client.get_id() ,&client));
		});

	};
	void connect_all_instance()
	{
		auto it = userPool_.begin();

		std::for_each(userPool_.begin(), userPool_.end(), [&](client_no_ssl& it)->void {
			it.activate_with_strand_no_err( &client_no_ssl::start, query_);
	 	});

	}
	void log_in_all_instance()
	{
		std::vector<bool>* userlogp = &userLoginTryPool_;
		int count = 0;
		std::for_each(userPool_.begin(), userPool_.end(), [&](client_no_ssl& it)->void {
			userlogp->at(count++) =  true;
			it.activate_with_strand_no_err( &client_no_ssl::login );
		});
		
	}
	template< class T>
	void task_assign(int id, T callback)
	{
		auto it = userPool_.at(id);
		it.activate_with_strand_no_err(callback);

	}
	void run_net()
	{
		boost::random::mt19937 rng;         // produces randomness out of thin air
		boost::random::uniform_int_distribution<> six(0,capacity_ - 1);

		int until = ( (capacity_ + 1) * 50 );
		int _workHash = 0;
		int _target = six(rng);
		int _subTarget = six(rng);

		while (_workHash++ < until)
		{
			_target = six(rng);
			_subTarget = six(rng);

			while (_target == _subTarget)
			{
				_target = six(rng);
				_subTarget = six(rng);
			}

			assign_task_to_client(_workHash++, _target, _subTarget);

		}
	}
	void set_target(int count, std::vector<std::string> & outContainer)
	{
		
			boost::random::mt19937 rng;         // produces randomness out of thin air
			boost::random::uniform_int_distribution<> targetGen(0, capacity_ - 1 );

			outContainer = std::vector<std::string>(count, "");
			std::vector<client_no_ssl>& refPool = userPool_;
			std::for_each(outContainer.begin(), outContainer.end(), [&](std::string& s)->void {
				int targetIndex = targetGen(rng);
				s = refPool.at(targetIndex).get_id();
			});
	}
	void assign_command_to_client(int _command, int _index)
	{
		boost::system::error_code err;
		client_no_ssl& client_ = userPool_[_index];

		if ((_command + _index) % 5 == 0)
		{
			BOOST_LOG_TRIVIAL(trace) << "Assign Get Message Command";


		}
		else if ((_command + _index) % 7 == 0)
		{
			BOOST_LOG_TRIVIAL(trace) << "Assign Get Ivt Command";


		}
		else if ((_command + _index) % 9 == 0)
		{
			BOOST_LOG_TRIVIAL(trace) << "Assign Get ConnectionRequest Command";


		}
		
		else
		{

		}
	}
	void assign_task_to_client( int _task, int _index, int _subTarget )
	{
		boost::system::error_code err;
		
		BOOST_LOG_TRIVIAL(trace) << "target : " + std::to_string(_index);
		
		boost::random::mt19937 rng;         // produces randomness out of thin air
		boost::random::uniform_int_distribution<> rangeGen(2 , 8);
		
		client_no_ssl& client = userPool_[_index];
		client_no_ssl& sub_client =  userPool_[_subTarget];

		if (!userLoginTryPool_[_index])
		{
			userLoginTryPool_[_index] = true;

			BOOST_LOG_TRIVIAL(trace) << "Assing Login Routine";
			client.activate_with_strand_no_err(&client_no_ssl::login);
			return;
		}
		if ( (( _task + _index) % 5) == 0)
		{
			BOOST_LOG_TRIVIAL(trace) << "Assign Invite Routine";
			client.activate_with_strand_no_err(&client_no_ssl::req_invite, sub_client.get_id() , 0);
		}
		
		else if (((_task + _index) % 7) == 0)
		{
			BOOST_LOG_TRIVIAL(trace) << "Assing Connection Request Routine";
			client.activate_with_strand_no_err(&client_no_ssl::req_connection , sub_client.get_id(), 0);
		}
		else if (((_task + _index) % 11) == 0)
		{
			BOOST_LOG_TRIVIAL(trace) << "Assing Create Table Routine";
			std::vector<std::string> participationsContainer;

			set_target(rangeGen(rng), participationsContainer);
			client.activate_with_strand_no_err(&client_no_ssl::req_create_table, std::vector<std::string>(participationsContainer));
		}
		else
		{
			BOOST_LOG_TRIVIAL(trace) << "Assing Message Send Routine";
			std::string text = "SEND FROM " + std::to_string(_index) + " TO " + std::to_string(_subTarget);
			
			client.activate_with_strand_no_err(&client_no_ssl::command_send_msg , 0 , 0 , text);
		}
	}
	void assign_connection(int _countPerSessionMin , int _countPerSessionMax)
	{
			
			boost::random::mt19937 rng;         // produces randomness out of thin air
			boost::random::mt19937 target;
			
			boost::random::uniform_int_distribution<> targetGen(0, capacity_ - 1 );
			boost::random::uniform_int_distribution<> capacity(_countPerSessionMin, _countPerSessionMax);
			int i = 0;
			int evg = (_countPerSessionMin + _countPerSessionMax) / 2;
			while (i++ <= evg * capacity_ )
			{
				int _firstTarget = targetGen(target);
				int _secondTarget = targetGen(target);

				while (_firstTarget == _secondTarget)
				{
					_firstTarget = targetGen(target);
					_secondTarget = targetGen(target);
				}

				client_no_ssl& _firstTargetSession = userPool_.at(_firstTarget);
				client_no_ssl& _secondTargetSession = userPool_.at(_secondTarget);
				connectionPool_.push_back(std::pair<std::string, std::string>(_firstTargetSession.get_id(), _secondTargetSession.get_id()));
			}
	}
	void view_connection()
	{
		std::for_each(connectionPool_.begin(), connectionPool_.end(), [](std::pair<std::string, std::string>& s)->void {
			std::cout << "User Pair :  " << s.first << " And  " << s.second << std::endl;
		});

	}

	void view_chat_list();

	void assign_chat_room(int _chatCount, int _sessionPerRoom)
	{
			boost::random::mt19937 rng;         // produces randomness out of thin air
			boost::random::uniform_int_distribution<> six(2, _sessionPerRoom);
			boost::random::uniform_int_distribution<> capacity(0, capacity_ - 1);
			int _assigned = 0;

			while (_assigned++ <= _chatCount * capacity_ / 2)
			{
				int _participations = six(rng);
				int idx = 0;
				roomAssign_.push_back(std::list<std::string > ());
				auto cont = &roomAssign_.back();

				for (idx = 0; idx < _participations; idx++)
				{
					int targetIndex = capacity(rng);
					client_no_ssl& target = userPool_.at(targetIndex);
					cont->push_back(target.get_id());
				}
			}
	}
		
public :
	void accept_command();
	void route_action();
	bool route_view_actions(std::vector< std::string > &);
	bool route_set_actions(std::vector <std::string>& cont);
	bool route_get_actions(std::vector<std::string>& cont);

private:

	int workHash_;
	std::vector<std::list<std::string > > roomAssign_;
	int capacity_;
	std::vector<bool> userLoginTryPool_;
	std::vector<client_no_ssl> userPool_;
	std::map< std::string, client_no_ssl* > userDic_;

	boost::asio::io_service bormService_;
	boost::asio::io_service clientService_;

	ip::tcp::resolver::iterator query_;

	boost::thread* bth;

	boost::asio::io_service::work cWork_;
	boost::asio::io_service::work bWork_;
	
	std::vector<std::string> stringCont_;
	
	std::vector<boost::thread*> cThreadPool_;
	std::vector<boost::thread*> bThreadPool_;

	boost::mutex taskAssignLock_;
	boost::mutex logLock_;

	std::vector<std::pair<std::string, std::string> > connectionPool_;
	std::map <std::string , client_no_ssl*  > userIndex_;

};