# MyShell

**required** packages:
<br>boost
<br>libreadline-dev

In directory `bin` there are custom commands (only `mycat` yet)<br>
To correctly run custom commands, `myshell` should be executed from building directory (`cd buil_dir && ./myshell`),<br>
otherwise add `bin` folder to path manually (`PATH.push_back("path_to_folder_with_command")`)