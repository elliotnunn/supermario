from os import path

def folder(default_search, user_input):
    if path.sep in user_input or user_input in ('..', '.'):
        return user_input
    else:
        the_path = path.dirname(path.dirname(path.abspath(__file__)))
        the_path = path.join(the_path, default_search, user_input)
        return the_path
