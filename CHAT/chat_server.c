/*
 * This is sample code generated by rpcgen.
 * These are only templates and you can use them
 * as a guideline for developing your own functions.
 */

#include "chat.h"
#include "chat_server_with_db.h"


result *
send_2_svc(send_params *argp, struct svc_req *rqstp)
{
    return chat_server_with_db::send_2_svc (argp, rqstp);
}

login_result *
login_2_svc(login_params *argp, struct svc_req *rqstp)
{
    return chat_server_with_db::login_2_svc (argp, rqstp);
}

result *
logout_2_svc(logout_params *argp, struct svc_req *rqstp)
{
    return chat_server_with_db::logout_2_svc (argp, rqstp);
}

receive_result *
receive_2_svc(receive_params *argp, struct svc_req *rqstp)
{
    return chat_server_with_db::receive_2_svc (argp, rqstp);
}

users_result *
users_2_svc(users_param *argp, struct svc_req *rqstp)
{
    return chat_server_with_db::users_2_svc (argp, rqstp);
}

result *
register_2_svc(register_params *argp, struct svc_req *rqstp)
{
    return chat_server_with_db::register_2_svc (argp, rqstp);
}