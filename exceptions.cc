better_exception::better_exception (const better_exception& cause,
				    const std::string& msg = "",)
    : std::exception (), _msg(msg)
{
/*
    _cause_stack.push_back (cause);

    _cause_stack.insert(_cause_stack.end(),
			cause._cause_stack.begin(), cause._cause_stack.end());
*/
}

