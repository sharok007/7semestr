#include <iostream>
#include <mutex>
#include <thread>
#include <vector>
#include <unistd.h>

#include "chat.h"

template <typename R, typename P> // Result, Param
R* user_man (CLIENT* cl, R* (*pfunc) (P *argp, CLIENT *clnt))
{
    char* username = new char[LOGIN_LEN];
    char* password = new char[PASSWORD_LEN];
    std::cout << "Username: ";
    std::cin >> username;
    std::cout << "Password: ";
    std::cin >> password;
    P param;
    param.login = new char[LOGIN_LEN];
    param.password = new char[PASSWORD_LEN];
    strcpy (param.login, username);
    strcpy (param.password, password);
    R* result;
    result = pfunc (&param, cl);
    if (result == NULL) {
	clnt_perror (cl, "call failed");
    }
    return result;
}

void login_menu(CLIENT* cl, int& cookie)
{
    std::size_t c;

    std::cout << "\n1. Log In\n2. Registration\n3. Exit\n";
    std::cin >> c;
    if (c == 1) {
	login_result* login_res = user_man <login_result, login_params> (cl, &login_2);
	std::cout << std::endl << login_res->res.descr << std::endl;
	if (login_res->res.code == OK) {
	    cookie = login_res->cookie;
	} else {
	    cookie = -1;
	}
    } else if (c == 2) {
	cookie = -1;
	result* register_res = user_man <result, register_params> (cl, &register_2);
	if (register_res->code == OK) {
	    std::cout << "Register success\n";
	} else {
	    std::cout << "Register fail: " << register_res->descr << std::endl;
	}
    } else if (c == 3) {
	exit (1);
    } else {
	std::cerr << "wrong parameter\n";
    }
}


void print_user_list (int& cookie, CLIENT* cl)
{
    users_result* users_res;
    users_param up;

    up.cookie = cookie;
    users_res = users_2 (&up, cl);
    if (users_res == NULL) {
	clnt_perror (cl, "call failed");
    }
    if (users_res->res.code == FAIL) {
	std::cout << users_res->res.descr << std::endl;
	return;
    }

    std::cout << '\n';
    for (int i=0; i<users_res->data.data_len; ++i) {
	std::cout << users_res->data.data_val[i].login << '\t';
	if (users_res->data.data_val[i].online == ONLINE) {
	    std::cout << "ONLINE\n";
	} else {
	    std::cout << "OFFLINE\n";
	}
    }
}

void send_message (int& cookie, CLIENT* cl)
{
    send_params sp;
    result* result;
    std::string recipient;
    std::string message;

    std::cout << "To: ";
    std::cin >> recipient;
    std::cout << "Message: ";
    getline (std::cin, message);
    getline (std::cin, message);
    sp.to = new char[LOGIN_LEN];
    sp.message = new char[MESSAGE_LEN];
    strcpy (sp.to, recipient.c_str());
    strcpy (sp.message, message.c_str());
    sp.cookie = cookie;

    result = send_2 (&sp, cl);
    if (result == NULL) {
	clnt_perror (cl, "call failed");
    }
    if (result->code == FAIL) {
	std::cout << result->descr << '\n';
    }
}

void logout (int& cookie, CLIENT* cl)
{
    logout_params lp;
    result* res;

    lp.cookie = cookie;
    res = logout_2 (&lp, cl);
    cookie = -1;
    while (cookie < 0) {
	login_menu (cl, cookie);
    }
}

class message_getter
{
public:

    static void check_new_messages (int& cookie, CLIENT* cl);
    static void read_messages ();

private:

    static std::mutex lock;
    static std::vector <std::pair <std::string, std::string>> messages;
};

std::vector <std::pair <std::string, std::string>> message_getter::messages;
std::mutex message_getter::lock; 

void message_getter::check_new_messages (int& cookie, CLIENT* cl)
{
    while (true) {
	receive_params rp;
	receive_result* recieve_res;

	rp.cookie = cookie;
	if (cookie < 0) {
	    sleep (5);
	    continue;
	}
	recieve_res = receive_2 (&rp, cl);
	if (recieve_res->res.code == FAIL) {
	    std::cout << recieve_res->res.descr << '\n';
	    return;
	}

	lock.lock();
	for (int i=0; i<recieve_res->data.data_len; ++i) {
	    messages.push_back (std::pair <std::string, std::string> (std::string (recieve_res->data.data_val[i].from),
	                                                              std::string (recieve_res->data.data_val[i].message)));
	}
	lock.unlock();
	if (recieve_res->data.data_len != 0) {
	    std::cout << "\t\t\t\t\t\t*New message*\n";
	}
	sleep (5);
    }
}

void message_getter::read_messages ()
{
    if (messages.size() == 0) {
	std::cout << "No new messages\n";
	return;
    }
    lock.lock();
    for (auto i = messages.begin(), e = messages.end(); i != e; ++i) {
	std::cout << i->first << ": " << i->second << "\n";
    }
    lock.unlock();
    messages.clear();
}

void user_functionality (int& cookie, CLIENT* cl)
{
    int c;
    while (true) {
	std::cout << "\n1. Print user list\n2. Send a message\n3. Get the message\n4. Log Out\n";
	std::cin >> c;
	switch (c) {
	case 1:
	    print_user_list (cookie, cl);
	    break;
	case 2:
	    send_message (cookie, cl);
	    break;
	case 3:
	    message_getter::read_messages();
	    break;
	case 4:
	    logout (cookie, cl);
	    break;
	default:
	    std::cerr << "wrong parameter\n";
	    break;
	}
    }
}


int main (int argc, char *argv[])
{
    int cookie = -1;
    CLIENT* cl;
    cl = clnt_create("localhost", RPC_CHAT, RPC_CHAT_VERSION_2, "udp");

    while (cookie < 0) {
	login_menu (cl, cookie);
    }

    std::thread message_thread (message_getter::check_new_messages, std::ref (cookie), cl);
    std::thread user_thread	  (user_functionality, std::ref (cookie), cl);
    if (message_thread.joinable()) message_thread.join();
    if (user_thread.joinable())	   user_thread.join();

    return 0;
}
