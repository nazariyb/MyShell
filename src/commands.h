#ifndef MYSHELL_COMMANDS_H
#define MYSHELL_COMMANDS_H

#include "utils.h"


struct Command
    {
    virtual EXIT_CODE run ( const VecStr & parsed_line ) = 0;

    virtual Args parse_arguments ( const VecStr & vecStr );
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
