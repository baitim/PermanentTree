#include "perm_tree.hpp"

int main()
{
    perm_tree::perm_tree_t<int> tree;

    char command;
    while(std::cin >> command) {
        if (!std::cin.good())
            return (std::cout << print_red("Error input, need command as char\n"), 1);

        int  key;
        char detach_command;
        std::list<int> detached_keys;
        switch (command) {
            case 'k':
                std::cin >> key;
                if (!std::cin.good())
                    return (std::cout << print_red("Error input, need key as int\n"), 1);

                tree.insert(key);
                break;

            case 's':
                std::cin >> detach_command;
                if (!std::cin.good() || detach_command != 'k')
                    return (std::cout << print_red("Error input, need detach command == \'k\'\n"), 1);

                std::cin >> key;
                if (!std::cin.good())
                    return (std::cout << print_red("Error input, need key as int\n"), 1);
                
                detached_keys = tree.detach_insert(key);
                for (auto i : detached_keys)
                    std::cout << i << " ";
                break;

            case 'r':
                tree.reset();
                break;

            default:
                return (std::cout << print_red("Error input, need command: \"k\", \"s\" or \"r\"\n"), 1);
        }

#ifdef DEBUG
        std::cout << tree << "\n";
#endif
    }
    std::cout << "\n";

    return 0;
}