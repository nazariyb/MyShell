#ifndef MYSHELL_COMMANDS_H
#define MYSHELL_COMMANDS_H

#include "utils.h"
#include <boost/program_options.hpp>


using OUT_TYPE = enum OUT_TYPE
    {
    OUT, ERROR
    };

struct Command
    {
    virtual EXIT_CODE run ( VecStr & parsed_line ) = 0;

    Args base_parsing ( VecStr & vecStr, od & desc, po::variables_map & vm, const VecStr & pos_args );

    virtual Args parse_arguments ( VecStr & vecStr ) = 0;

    static void parse_redirections ( VecStr & vecStr, Args & args );
    std::ofstream out_file;

    void log ( const std::string & message, OUT_TYPE out_type, bool is_end );

    void log ( const std::string & message, OUT_TYPE out_type, bool is_end, const std::string & dest );

private:

    };


struct mErrno : public Command
    {
    EXIT_CODE run ( VecStr & parsed_line ) override;

    Args parse_arguments ( VecStr & vecStr ) override;
    };


struct mPwd : public Command
    {
    EXIT_CODE run ( VecStr & parsed_line ) override;

    Args parse_arguments ( VecStr & vecStr ) override;
    };


struct mCd : public Command
    {
    EXIT_CODE run ( VecStr & parsed_line ) override;

    Args parse_arguments ( VecStr & vecStr ) override;
    };


struct mExit : public Command
    {
    EXIT_CODE run ( VecStr & parsed_line ) override;

    Args parse_arguments ( VecStr & vecStr ) override;
    };


struct mEcho : public Command
    {
    EXIT_CODE run ( VecStr & parsed_line ) override;

    Args parse_arguments ( VecStr & vecStr ) override;
    };


struct mExport : public Command
    {
    EXIT_CODE run ( VecStr & parsed_line ) override;

    Args parse_arguments ( VecStr & vecStr ) override;
    };


#endif //MYSHELL_COMMANDS_H
