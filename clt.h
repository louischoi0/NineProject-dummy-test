#pragma once
#include "stdafx.h"

#include "nof.h"
#include "packer.h"

#define max_length 1024

typedef const boost::system::error_code& constBoostErr;
typedef boost::function<void(boost::asio::ip::tcp::resolver::iterator)> CallbackWithIter;

const std::string id = "test_id10";
const std::string pw = "test_pw10";
typedef struct {
	int table_key;
	int host_id_size;
	int participations;
}TableDescriptor;
typedef struct
{
	int key_allocated;
}CreateTableResponse;
typedef struct
{
	int table_key;
	int sender_id_size;
	boost::posix_time::ptime ptime;

}IvtResponse;

typedef struct
{
	int sender_id_size;
	boost::posix_time::ptime ptime;
}ConcResponse;
typedef struct
{
	int host_id_size;
	int participations;

}CreateTableReq;
typedef struct
{
	int id_size;
	int id_key;
}UserDescriptor;

typedef struct
{
	int sender_id_size;
	int receiver_id_size;
	boost::posix_time::ptime time;

}ConcRequest;

typedef struct
{
	int sender_id_size;
	int table_key;

}GetInviteRequest;
typedef struct
{
	int table_key;
	int target_id_size;
	
}IvtReq;
typedef struct msg
{
	int table_key;
	int sender;

	short sz;

	int uncheck;
	boost::posix_time::ptime time;
} msReq;
typedef struct
{
	int tableKey;

	short userIndex;
	short text_lenth;

	char time[32];
	int arrived;
	short check;

	int sz;

}msOut;
typedef struct
{
	int sizeof_id;
	int sizeof_pw;

} rqlog;
typedef struct
{
	int table_key;
	int sd;

	int time;

	int size;
	short init;



}  msrq;

enum eComd
{
	Ivt = 1,
	Msg = 2,
	Ctb = 5,
	Conc = 7,

	ReqLog = 20,
	ReqLogOut = 21
};

typedef struct boost::asio::ip::tcp::socket nsocket;
typedef struct pc_hd;

class client_base : public boost::enable_shared_from_this<client_base>
{
public:
	client_base() :
		nf(NULL), init(false)
	{};

	client_base(notifier* n) :
		nf(n), init(false)
	{};

	void set_notifier(notifier* n) { nf = n; init = true; };
	bool get_init() { return init; };


protected:
	bool init;
	notifier *nf;

};

typedef boost::function<void(boost::system::error_code&)> function_werr;

class client_no_ssl : public client_base
{
	static int index;

public:
	client_no_ssl(boost::asio::io_service& io, boost::asio::ip::tcp::resolver::iterator endpoint_iterator)
		:
			socket_(io),
			i(index++),
			io_(io),
			query_(endpoint_iterator),
			pc(&buf),
			strand_(io),
			logged(false),
			hBuffer_(sizeof(pchd)),
			source_(1024)
			
	{
		convert_loging_info_string();
		BOOST_LOG_TRIVIAL(trace) << "Connect Session Constructed" ;
	}

	client_no_ssl(const client_no_ssl& s) :
		socket_(s.io_),
		i(index++),
		io_(s.io_),
		query_(s.query_),
		pc(&buf),
		strand_(s.io_),
		logged(false),
		hBuffer_(sizeof(pchd)),
		source_(1024)
	{
		convert_loging_info_string();
		BOOST_LOG_TRIVIAL(trace) << "Connect  Session Copied";
	}

	~client_no_ssl()
	{
		BOOST_LOG_TRIVIAL(trace) << "Session Aborted";
	}

	void convert_loging_info_string()
	{
		id_request = id + std::to_string(i);
		pw_request = pw + std::to_string(i);

		id_request = boost::locale::conv::to_utf<char>(id_request , "EUC-KR");
		pw_request = boost::locale::conv::to_utf<char>(pw_request, "EUC-KR");
		
	}

	void test(boost::system::error_code& err, boost::asio::ip::tcp::resolver::iterator e)
	{
		boost::asio::connect( socket_ ,  e);
	}

	template < class T , class A0 , class A1 >
	void activate_with_strand( const boost::system::error_code& err , T callback, A0 args0 , A1 args1 )
	{
		io_.post(strand_.wrap(boost::bind(callback, this , args0 , args1)));
	}

	template <class T, class A0 >
	void activate_with_strand(const boost::system::error_code& err, T callback, A0 args0)
	{
		io_.post(strand_.wrap(boost::bind(callback, this, err, args0)));
	}

	template <class T>
	void activate_with_strand(const boost::system::error_code& err, T callback )
	{
		io_.post(strand_.wrap(boost::bind(callback, this, err)));
	}

	template <class T>
	void activate_with_strand_no_err(T callback)
	{
		io_.post(strand_.wrap(boost::bind(callback, this)));
	}

	void req_invite_send_tail(const boost::system::error_code& err, std::string target, int tableKey)
	{
		IvtReq rq = {};

		rq.target_id_size = target.size();
		rq.table_key = tableKey;

		target = boost::locale::conv::to_utf<char>( target, "EUC-KR" );

		boost::asio::write(socket_, boost::asio::buffer(&rq, sizeof(IvtReq)));
		boost::asio::write(socket_, boost::asio::buffer(target.c_str(), target.size()));
	
	}
	template <class T, class A0>
	void activate_with_strand_no_err(T callback, A0 args)
	{
		io_.post(strand_.wrap(boost::bind(callback, this, args)));
	}
	template <class T, class A0 , class A1>
	void activate_with_strand_no_err(T callback, A0 arg0, A1 arg1)
	{
		io_.post(strand_.wrap(boost::bind(callback, this, arg0, arg1)));
	}
	template <class T, class A0, class A1 , class A2>
	void activate_with_strand_no_err(T callback , A0 arg0 , A1 arg1 , A2 arg2)
	{
		io_.post(strand_.wrap(boost::bind(callback, this ,arg0, arg1, arg2)));
	}
	void start(boost::asio::ip::tcp::resolver::iterator endpoint_iterator)
	{
		BOOST_LOG_TRIVIAL(trace) << "Client Connection request";
		boost::asio::connect(socket_, endpoint_iterator);
	}
	void routine(const boost::system::error_code& err)
	{
		std::cout << "completed" << endl;
	}
	void command_send_msg(int tableKey ,int senderKey ,std::string text)
	{
		pchd h = {};
		h.sz = sizeof(msReq);
		h.comd = 80;
		h.actionOrCommand = 0;
		boost::system::error_code ell;

		boost::asio::write(socket_, boost::asio::buffer(&h, sizeof(h)), ell);
		req_send_msg(ell, tableKey , senderKey , text);

	}
	void req_send_msg(const boost::system::error_code& err, int tableKey, int senderKey, std::string text)
	{
		msReq r = {};
		r.table_key = tableKey;
		r.sender = senderKey;
		r.sz = text.size();
		r.uncheck = 1;

		r.time = boost::posix_time::second_clock::local_time();
		
		boost::system::error_code ell;

		boost::asio::write(socket_, boost::asio::buffer(&r, sizeof(msReq)), ell);
		req_send_msg_handle(ell, text);
	}
	void req_create_table(std::vector<std::string>& cont)
	{
		pBuffer p(256);
		pchd h = {};
		CreateTableReq rq_body = {};

		h.comd = 83;
		h.actionOrCommand = 0;
		h.sz = sizeof(CreateTableReq);

		p.allocate_some(&h , sizeof(pchd));

		rq_body.host_id_size = user_name.size();
		rq_body.participations = cont.size();

		p.allocate_some(&rq_body, sizeof(CreateTableReq));
				
		for (auto i = cont.begin() ; i != cont.end() ; i++)
		{
			UserDescriptor us;
			*i = boost::locale::conv::to_utf<char>(*i , "EUC-KR");
			
			us.id_size = (*i).size();
			
			p.allocate_some( &us, sizeof(UserDescriptor) );
			p.allocate_some( (void*)(*i).c_str() , (*i).size() );

		}

		boost::system::error_code ell;

		boost::asio::write(socket_, boost::asio::buffer(p.src(), p.written_size()), ell);
		check_table_created(ell);
	}
	void check_table_created(const boost::system::error_code& err)
	{

		if (!err)
		{
			pBuffer p(sizeof(CreateTableResponse));
			boost::asio::read(socket_, boost::asio::buffer(p.src_inc_alloc(sizeof(CreateTableResponse)), sizeof(CreateTableResponse)));
			CreateTableResponse* res = (CreateTableResponse*)p.src();
			
			std::cout << "TABLE CREATED : " << res->key_allocated << std::endl;
		}

		else
		{
		}

	}
	void req_send_msg_handle(const boost::system::error_code& err, std::string text)
	{
		boost::system::error_code ell;
		
		//text = boost::locale::conv::to_utf<char>( text , "EUC-KR");
		boost::asio::write(socket_, boost::asio::buffer(text.c_str(), text.size()), ell);
	}
	void check_receive(const boost::system::error_code& err )
	{


	}
	void login()
	{
		req_login();
	}
	void req_login()
	{
		try
		{
			user_name = id;

			pchd h = {};
			h.comd = eComd::ReqLog;

			rqlog r = {};
			r.sizeof_id = id_request.size();
			r.sizeof_pw = pw_request.size();
			
			h.sz = sizeof(rqlog);

			boost::system::error_code err;

			boost::asio::write(socket_, boost::asio::buffer(&h, sizeof(pchd)) , err);
			req_login_handle(err  ,r, id_request, pw_request);
		
		}

		catch (boost::system::error_code& err)
		{
			BOOST_LOG_TRIVIAL(info) << err;
		}
	}
	void req_login_handle(const boost::system::error_code& err, rqlog r, std::string id, std::string pw)
	{
		char buf[512];
		char* buff = buf;
		int size = sizeof(rqlog) + id.size() + pw.size();

		memcpy(buff, &r, sizeof(rqlog));
		buff += sizeof(rqlog);

		memcpy(buff, id.c_str(), id.size());
		buff += id.size();

		memcpy(buff, pw.c_str(), pw.size());
		buff += pw.size();	

		boost::system::error_code lerr;
		boost::asio::write(socket_, boost::asio::buffer(buf, size), lerr);
		get_login_result(lerr);
	}
	void get_login_result(const boost::system::error_code& err )
	{
		char info[512];
		logged = true;
		
		boost::system::error_code lerr;
		bool check;
		boost::asio::read(socket_ , boost::asio::buffer( &check , sizeof(bool) ), lerr);
		
		BOOST_LOG_TRIVIAL(trace) << id_request + " : LOGGED";
	
	}

	void check_recieved_packet(const boost::system::error_code& err, pchd& h)
{

	}

	void read_body_handle(const boost::system::error_code& err, int bytes_transferred)
	{
		pchd h = pc.front();

		int i = h.sz;
	}

	void req_connection(std::string target , int rank )
	{
		pchd h = {};
		h.sz = sizeof(ConcRequest);
		h.comd = 81;

		h.actionOrCommand = 0;

		boost::system::error_code ell;
		boost::asio::write(socket_, boost::asio::buffer(&h, sizeof(pchd)), ell);
				
		ConcRequest rq = {};
		rq.receiver_id_size= target.size();

		boost::asio::write(socket_, boost::asio::buffer(&rq , sizeof(ConcRequest)));
		boost::asio::write(socket_, boost::asio::buffer(target.c_str(), target.size()), ell);
	
	}


	void req_invite( std::string target , int tableKey )
	{
		pchd h = {};
		h.comd = 82;
		h.actionOrCommand = 0;
		h.sz = sizeof(IvtReq);
		boost::system::error_code ell;

		boost::asio::write(socket_, boost::asio::buffer(&h, sizeof(pchd)), ell);
		req_invite_send_tail(ell, target, tableKey);

	}


	void req_connection_send_tail(const boost::system::error_code& err , std::string target , int rank)
	{
	}

	void check_okay(const boost::system::error_code& err)
	{
		//char A;
		//boost::asio::read(socket_, boost::asio::buffer(&A, 1));
	
	}
	void connect(const boost::system::error_code& err)
	{
		BOOST_LOG_TRIVIAL(trace) << "Client Connection handle";

		if (!err)
		{
			routine(err);
		
		}

		else
		{
			std::cerr << err.message() << std::endl;

		}


	}
	void write_haeder_handle(const boost::system::error_code& err, std::string str)
	{


	}
	void write_handle(const boost::system::error_code& err, std::string str)
	{
		if (!err)
		{


		}

		else
		{
			std::cerr << err.message() << std::endl;

		}


	}
	void reconnect()
	{
		socket_.close();
		socket_ = nsocket(io_);
		start(query_);
	}
	void disconnect()
	{
		socket_.close();
		socket_ = nsocket(io_);
	}
	void get_conc_req()
	{
		boost::system::error_code ell;
		pchd h = {};
		h.actionOrCommand = 1;
		h.comd = 72;

		pBuffer nameBuffer(256);
		
		boost::asio::write(socket_ ,boost::asio::buffer(&h , sizeof(h)), ell);
		boost::asio::read(socket_, boost::asio::buffer(hBuffer_.src() , sizeof(pchd)), ell);
		
		std::list<ConcRequest> list;
		std::list<std::string > slist;
		
		pchd* hd =(pchd*) hBuffer_.src();
		char id[32];
		for (int i = 0; i < hd->count; i++)
		{
			ConcRequest hiter = {};

			boost::asio::read(socket_, boost::asio::buffer(&hiter, sizeof(ConcRequest)));
			boost::asio::read(socket_, boost::asio::buffer(id, hiter.sender_id_size));
			
			slist.push_back(std::string(id, hiter.sender_id_size));
		}

	}

	void get_ivt_req()
	{
		boost::system::error_code ell;
		pchd h = {};
		h.sz = 0;
		h.actionOrCommand = 1;
		h.comd = 71;
		
		boost::asio::write(socket_, boost::asio::buffer(&h, sizeof(pchd)));
		boost::asio::read(socket_, boost::asio::buffer(hBuffer_.src() , sizeof(pchd)), ell);
		boost::asio::read(socket_, boost::asio::buffer(source_.src(), sizeof(IvtResponse)), ell);
		pchd* hd = (pchd*)hBuffer_.src();
		IvtResponse* ivtDes = (IvtResponse*)source_.src();
		boost::asio::read(socket_, boost::asio::buffer(source_.src(), ivtDes->sender_id_size), ell);

		std::string hostName(source_.src(), ivtDes->sender_id_size);
		BOOST_LOG_TRIVIAL(trace) << "Invited By " << hostName;

	}
	void get_connection_request()
	{
		boost::system::error_code ell;
		pchd h = {};
		h.sz = 0;
		h.actionOrCommand = 1;
		h.comd = 72;
		

	}
	void get_new_messages()
	{
		boost::system::error_code ell;
		pchd h = {};
		h.sz = 0;
		h.actionOrCommand = 1;
		h.comd = 70;
	


	}

	void get_friend_list()
	{
		boost::system::error_code ell;
		pchd h = {};
		h.sz = 0;
		h.actionOrCommand = 1;
		h.comd = 73;

		boost::asio::write(socket_, boost::asio::buffer(&h, sizeof(pchd)), ell);
		boost::asio::read(socket_, boost::asio::buffer(hBuffer_.src(), sizeof(pchd)), ell);

		pchd hd = *(pchd*)hBuffer_.src();
		std::list<std::string> fList;

		for (int i = 0; i < hd.count; i++)
		{
			boost::asio::read(socket_ , boost::asio::buffer( source_.src() , sizeof(UserDescriptor)), ell);
			UserDescriptor userDes = *(UserDescriptor*)source_.src();
				
			boost::asio::read(socket_, boost::asio::buffer(source_.src(), userDes.id_size) , ell);

			std::string element(source_.src(), userDes.id_size);
			element = boost::locale::conv::to_utf<char>(element, "EUC-KR");
			
			BOOST_LOG_TRIVIAL(trace) << "Friend : " << element;
		}

		BOOST_LOG_TRIVIAL(trace) << "Get Friend Command End";
	}

	void handle_header_sended(pchd& h, pBuffer& buffer)
	{
		if (h.comd == 71)
		{
			parsing_invite_info(h.count, buffer);
		}
		
	}
	void parsing_invite_info(int count , pBuffer& buffer)
	{
		int allocated_size =  count * sizeof(GetInviteRequest);
		boost::asio::read(socket_, boost::asio::buffer( buffer.src() , allocated_size));
	}


	void request_new_connection(const boost::system::error_code& err)
	{

	}

	void get_new_connection_requested(const boost::system::error_code& err)
	{
//		pc.decode_haeder(err);

	}

	void get_conc_buffer_in_serial(const boost::system::error_code& err)
	{
		pchd* h = (pchd*)pc.data();

		std::list<msOut> mlist;
		std::list<std::string> slist;

		char text[128];

		for (int i = 0; i < h->count; i++)
		{
			mlist.push_back(msOut());
			slist.push_back(std::string());

			auto miter = mlist.back();
			auto siter = slist.back();

			boost::asio::read(socket_, boost::asio::buffer(&(miter), sizeof(msOut)));
			boost::asio::read(socket_, boost::asio::buffer(text, miter.sz));

			BOOST_LOG_TRIVIAL(trace) << "T : " << miter.tableKey << "  > " << text;

			siter = text;

		}
		return;
	}


	void get_msg_buffer_in_serial(const boost::system::error_code& err, int size)
	{
		pchd* h = (pchd*)pc.data();
		
		std::list<msOut> mlist;
		std::list<std::string> slist;

		char text[128];

		for (int i = 0; i < h->count; i++)
		{
			mlist.push_back(msOut());
			slist.push_back(std::string());
			
			auto miter = mlist.back();
			auto siter = slist.back();

			boost::asio::read(socket_, boost::asio::buffer( &(miter), sizeof(msOut)));
			boost::asio::read(socket_, boost::asio::buffer(  text , miter.sz));
			
			BOOST_LOG_TRIVIAL(trace) << "T : " << miter.tableKey << "  > " << text;

			siter = text;
		
		}


		return;

	}


	void send_handle(int comd , int sz , const char* buff)
	{
	
	}

	void close()
	{
		pchd h = {};
		h.comd = 99;
		h.sz = 0;

		boost::asio::write(socket_, boost::asio::buffer(&h, sizeof(pchd)));

		socket_.close();
		delete this;
	}
	bool get_logged() { return logged; };
	std::string get_id() { return id_request; };
private:
	nsocket socket_;
	int i;
	Packer pc;
	bool logged;

	boost::array<char, 1024> buf;
	boost::asio::io_service& io_;
	boost::asio::ip::tcp::resolver::iterator query_;

	std::string id_request;
	std::string pw_request;

	std::string user_name;

	boost::asio::io_service::strand strand_;
	pBuffer source_;
	pBuffer hBuffer_;
};

void accept_command(std::vector<std::string>& cont, std::vector<client_no_ssl*>& C, boost::asio::io_service* io);
void route_action(std::vector<std::string>& cont, std::vector<client_no_ssl*>& C, std::vector<boost::asio::io_service::strand>& S, boost::asio::io_service* io);

