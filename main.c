void assembler_main_routine(int file_count, char ** file_names);
int main(int argc, char ** argv) {
    assembler_main_routine(argc-1,argv++);
    return 0;
}