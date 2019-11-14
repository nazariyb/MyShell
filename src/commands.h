#ifndef MYSHELL_COMMANDS_H
#define MYSHELL_COMMANDS_H

#include "utils.h"


using OUT_TYPE = enum OUT_TYPE
    {
    OUT, ERROR
    };

struct Command
    {
    virtual EXIT_CODE run ( const VecStr & parsed_line ) = 0;
    virtual Args parse_arguments ( const VecStr & vecStr );

    std::ofstream out_file;

    void log ( const std::string & message, OUT_TYPE out_type, bool is_end );

    void log ( const std::string & message, OUT_TYPE out_type, bool is_end, const std::string & dest );

private:

    };


struct mErrno : public Command
    {
    EXIT_CODE run ( const VecStr & parsed_line ) override;
    //    void parse_arguments(const VecStr& vecStr, Args & args);
    };


struct mPwd : public Command
    {
    EXIT_CODE run ( const VecStr & parsed_line ) override;
    //    void parse_arguments(const VecStr& vecStr, Args & args);
    };


struct mCd : public Command
    {
    EXIT_CODE run ( const VecStr & parsed_line ) override;

    Args parse_arguments ( const VecStr & vecStr ) override;
    };


struct mExit : public Command
    {
    EXIT_CODE run ( const VecStr & parsed_line ) override;

    Args parse_arguments ( const VecStr & vecStr ) override;
    };


struct mEcho : public Command
    {
    EXIT_CODE run ( const VecStr & parsed_line ) override;

    Args parse_arguments ( const VecStr & vecStr ) override;
    };


struct mExport : public Command
    {
    EXIT_CODE run ( const VecStr & parsed_line ) override;

    Args parse_arguments ( const VecStr & vecStr ) override;
    };


#endif //MYSHELL_COMMANDS_H
