#include "os/rtstreams.hpp"
#include "os/fosi.h"

namespace rt_std
{

    printstream cout;
    
    basic_streams::~basic_streams()
    {}

    basic_ostreams& basic_ostreams::write( const char * c, streamsize n )
    {
        buf.sputn( c, n);
        return *this;
    }

    basic_ostreams& basic_ostreams::put( char c )
    {
        buf.sputc( c );
        return *this;
    }

    basic_istreams& basic_istreams::read(char* c, streamsize n)
    {
        buf.sgetn(c,n);
        return *this;
    }

    basic_istreams::streamsize basic_istreams::readsome(char* c, streamsize n)
    {
        return buf.sgetn(c,n);
    }

    basic_istreams& basic_istreams::get( char& c )
    {
        c = buf.sgetc();
        return *this;
    }

    basic_istreams& basic_istreams::get( char*c ,streamsize n, char delim)
    {
        streamsize i=0;
        int res;
        do {
            res = buf.sgetc();
            c[i++] = res;
        } while ( res != EOF && c[i-1] != delim && i != n );

        if ( i == n)
            c[i] = 0;
        else
            c[i-1] = 0;

        return *this;
    }

    basic_istreams& basic_istreams::get( char*c,  char delim)
    {
        streamsize i=0;
        int res;
        do {
            res = buf.sgetc();
            c[i++] = res;
        } while ( res != EOF && c[i-1] != delim );
        
        c[i-1] = 0;

        return *this;
    }
    
    basic_ostreams & endl( basic_ostreams & s )
    {
        return s << '\n';
    }
    
    basic_ostreams& basic_ostreams::operator<<( int i )
    {
        std::string result( int_to_string( i ) );
        write(result.c_str(), result.length() );
        return *this;
    }

    basic_ostreams& basic_ostreams::operator<<( long l )
    {
        std::string result( int_to_string( l ) );
        write(result.c_str(), result.length() );
        return *this;
    }

    basic_ostreams& basic_ostreams::operator<<( char c )
    {
        buf.sputc( c );
        return *this;
    }

    basic_ostreams& basic_ostreams::operator<<( char * c )
    {
        buf.sputn( c, strlen(c) );
        return *this;
    }

    basic_ostreams& basic_ostreams::operator<<( basic_ostreams & ( *f ) ( basic_ostreams & ) )
    {
        return ( *f ) ( *this );
    }

    basic_ostreams& basic_ostreams::operator<<( double f )
    {
        std::string result( float_to_string( f ) );
        write( result.c_str(), result.length() );
        return *this;
    }

    basic_ostreams& basic_ostreams::operator<<( std::string s )
    {
        write( s.c_str(), s.length() );
        return *this;
    }

    basic_ostreams& basic_ostreams::operator<<( unsigned int u )
    {
        std::string result( unsigned_int_to_string( u ) );
        write( result.c_str(), result.length() );
        return *this;
    }

    basic_istreams& basic_istreams::operator>>( int &i )
    {
        std::string result;
        result.reserve(10);
        int i_res = buf.sgetc();
        while ( (i_res != EOF) && (i_res != ' ') )
		{
              result += char(i_res);
              i_res = buf.sgetc();
        }
        i = string_to_int( result );
        return *this;
    }

    basic_istreams& basic_istreams::operator>>( char &c )
    {
        c = buf.sgetc();
        return *this;
    }

    basic_istreams& basic_istreams::operator>>( double &f )
    {
        std::string result;
        result.reserve(40);
        int i_res = buf.sgetc();
        while ( (i_res != EOF) && (i_res != ' ') )
		{
              result += char(i_res);
              i_res = buf.sgetc();
        }
        //f = string_to_float( result );
        f = -1;
        return * this;
    }

    basic_istreams& basic_istreams::operator>>( std::string &s )
    {
        std::string result;
        int i_res; 
        do {
              i_res = buf.sgetc();
              if (i_res == EOF )
                  break;
              result += char(i_res);
        } while ( i_res != ' ' );
        
        s = result;
        
        return *this;
    }

    basic_istreams& basic_istreams::operator>>( unsigned int &u )
    {
        std::string result;
        result.reserve(10);
        int i_res = buf.sgetc();
        while ( (i_res != EOF) && (i_res != ' ') )
		{
              result += char(i_res);
              i_res = buf.sgetc();
        }
        
        u = string_to_unsigned_int( result );
        
        return *this;
    }

    basic_istreams::~basic_istreams()
    {}

    basic_ostreams::~basic_ostreams()
    {}


    printstream::~printstream()
    {}

    stringstreams::~stringstreams()
    {}


}
