#include <iostream>
#include <cstdlib>
#include <cstring>

#include <sstream>
#include <list>
#include <stack>
#include <map>
#include <string>
#include <vector>
#include <iterator>

#include "sharedm.h"


using namespace std;

const int LEFT_ASSOC  = 0;
const int RIGHT_ASSOC = 1;

typedef map< string, pair< int,int > > OpMap;
typedef vector<string>::const_iterator cv_iter;
typedef string::iterator s_iter;

const OpMap::value_type assocs[] =
{  OpMap::value_type( "+", make_pair<int,int>( 0, 0 ) ),
   OpMap::value_type( "-", make_pair<int,int>( 0, 0 ) ),
   OpMap::value_type( "*", make_pair<int,int>( 5, 0 ) ),
   OpMap::value_type( "/", make_pair<int,int>( 5, 0 ) )
};

const OpMap opmap( assocs, assocs + sizeof( assocs ) / sizeof( assocs[ 0 ] ) );

bool isParenthesis( const string& token)
{
    return token == "(" || token == ")";
}

bool isOperator( const string& token)
{
    return token == "+" || token == "-" ||
            token == "*" || token == "/";
}

bool isAssociative( const string& token, const int& type)
{
    const pair<int,int> p = opmap.find( token )->second;
    return p.second == type;
}

int cmpPrecedence( const string& token1, const string& token2 )
{
    const pair<int,int> p1 = opmap.find( token1 )->second;
    const pair<int,int> p2 = opmap.find( token2 )->second;

    return p1.first - p2.first;
}

bool infixToRPN( const vector<string>& inputTokens,
                 const int& size,
                 vector<string>& strArray )
{
    bool success = true;

    list<string> out;
    stack<string> stack;

    for ( int i = 0; i < size; i++ )
    {
        const string token = inputTokens[ i ];

        if ( isOperator( token ) )
        {
            const string o1 = token;

            if ( !stack.empty() )
            {
                string o2 = stack.top();

                while ( isOperator( o2 ) &&
                        ( ( isAssociative( o1, LEFT_ASSOC ) &&  cmpPrecedence( o1, o2 ) == 0 ) ||
                          ( cmpPrecedence( o1, o2 ) < 0 ) ) )
                {
                    stack.pop();
                    out.push_back( o2 );

                    if ( !stack.empty() )
                        o2 = stack.top();
                    else
                        break;
                }
            }

            stack.push( o1 );
        }
        else if ( token == "(" )
        {
            stack.push( token );
        }

        else if ( token == ")" )
        {
            string topToken  = stack.top();

            while ( topToken != "(" )
            {
                out.push_back(topToken );
                stack.pop();

                if ( stack.empty() ) break;
                topToken = stack.top();
            }

            if ( !stack.empty() ) stack.pop();

            if ( topToken != "(" )
            {
                return false;
            }
        }
        else
        {
            out.push_back( token );
        }
    }

    while ( !stack.empty() )
    {
        const string stackToken = stack.top();

        if ( isParenthesis( stackToken )   )
        {
            return false;
        }

        out.push_back( stackToken );
        stack.pop();
    }

    strArray.assign( out.begin(), out.end() );

    return success;
}


double RPNtoDouble( vector<string> tokens )
{
    stack<string> st;

    for ( int i = 0; i < static_cast<int>(tokens.size()); ++i )
    {
        const string token = tokens[ i ];
        if ( !isOperator(token) )
        {
            st.push(token);
        }
        else
        {
            double result =  0.0;

            const string val2 = st.top();
            st.pop();
            const double d2 = strtod( val2.c_str(), NULL );

            if ( !st.empty() )
            {
                const string val1 = st.top();
                st.pop();
                const double d1 = strtod( val1.c_str(), NULL );

                result = token == "+" ? d1 + d2 :
                                        token == "-" ? d1 - d2 :
                                                       token == "*" ? d1 * d2 :
                                                                      d1 / d2;
            }
            else
            {
                if ( token == "-" )
                    result = d2 * -1;
                else
                    result = d2;
            }

            ostringstream s;
            s << result;
            st.push( s.str() );
        }
    }

    return strtod( st.top().c_str(), NULL );
}

vector<string> getExpressionTokens( const string& expression )
{
    vector<string> tokens;
    string str = "";

    for ( int i = 0; i < static_cast<int>(expression.length()); ++i )
    {
        const string token( 1, expression[ i ] );

        if ( isOperator( token ) || isParenthesis( token ) )
        {
            if ( !str.empty() )
            {
                tokens.push_back( str ) ;
            }
            str = "";
            tokens.push_back( token );
        }
        else
        {
            if ( !token.empty() && token != " " )
            {
                str.append( token );
            }
            else
            {
                if ( str != "" )
                {
                    tokens.push_back( str );
                    str = "";
                }
            }
        }
    }

    return tokens;
}

void error(string err){
    cout << err << endl;
    exit(-1);
}

int main()
{
    key_t keyShared, keySemaphor;
    SharedMem *shared;
    int shmid, semid;

    keyShared = ftok("memory", 'A');
    keySemaphor = ftok("semaphor", 'A');

    //Сумафор
    semid = semget(keySemaphor, 1, 0);
    if(semid < 0)
        error("Can not get semaphore");

    //Разделяемая память
    shmid = shmget(keyShared, sizeof (memoryBuffer), 0666);
    if(shmid < 0)
        error("Can not get shared memory segment");

    shared = static_cast<memoryBuffer*>(shmat(shmid, 0, 0));
    if(shared == NULL)
        error("Shared memory attach error");

    while(shared->msg_type != FINISH_MEM){
        if (shared->msg_type == RECEIVE_STR && semctl (semid, 0, GETVAL, 0) == 0){
            //Блокируем доступ к разделяемой памяти
            semctl (semid, 0, SETVAL, 1);
            //Считываем из памяти выражение, считаем его и кладём результат в память
            if(shared->msg_type == RECEIVE_STR){
                string s;
                s.push_back('(');
                s = s + shared->buff;
                s.push_back(')');

                vector<string> tokens = getExpressionTokens( s );
                vector<string> rpn;

                if ( infixToRPN( tokens, static_cast<int>(tokens.size()), rpn ) ){
                    shared->result = RPNtoDouble( rpn );
                    shared->msg_type = WRITE_STR;
                    strcpy(shared->status, "CALCULATE");
                    cout << "STATUS: " << shared->status << endl;
                }
                else{
                    cout << "Mis-match in parentheses" << endl;
                    shared->msg_type = EMPTY_STR_MEM;
                    strcpy(shared->status, "ERROR");
                    cout << "STATUS: " << shared->status << endl;
                    continue;
                }
            }
            //Снимаем блокировку
            semctl(semid, 0, SETVAL, 0);
        }
        else
            continue;
    }

    shmdt(shared);

    return 0;
}
