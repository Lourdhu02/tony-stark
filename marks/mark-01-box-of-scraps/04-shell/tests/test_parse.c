#include "shell.h"
#include "test.h"

/* 1 if cmd's argv is exactly the NULL-terminated list `want`. */
static int argv_is(const Command *cmd, const char **want)
{
    int n = 0;
    while (want[n]) n++;
    if (cmd->argc != n || cmd->argv == NULL || cmd->argv[n] != NULL)
        return 0;
    for (int i = 0; i < n; i++)
        if (!cmd->argv[i] || strcmp(cmd->argv[i], want[i]) != 0)
            return 0;
    return 1;
}

#define ARGV(...) ((const char *[]){__VA_ARGS__, NULL})

static int str_is(const char *got, const char *want)
{
    return got && want && strcmp(got, want) == 0;
}

TEST(blank_lines)
{
    Pipeline p;
    REQUIRE(parse_line("", &p) == 0);
    CHECK(p.ncmds == 0);
    pipeline_free(&p);
    REQUIRE(parse_line("   \t  ", &p) == 0);
    CHECK(p.ncmds == 0);
    pipeline_free(&p);
}

TEST(simple_command)
{
    Pipeline p;
    REQUIRE(parse_line("  ls   -l\t-a  ", &p) == 0);
    REQUIRE(p.ncmds == 1);
    CHECK(argv_is(&p.cmds[0], ARGV("ls", "-l", "-a")));
    CHECK(p.cmds[0].in_file == NULL && p.cmds[0].out_file == NULL);
    CHECK(p.background == 0);
    pipeline_free(&p);
}

TEST(quotes)
{
    Pipeline p;
    REQUIRE(parse_line("echo \"hello   world\" 'it''s' \"\"", &p) == 0);
    REQUIRE(p.ncmds == 1);
    CHECK(argv_is(&p.cmds[0], ARGV("echo", "hello   world", "its", "")));
    pipeline_free(&p);
}

TEST(quotes_glue_to_words)
{
    Pipeline p;
    REQUIRE(parse_line("echo a\"b c\"d'e'", &p) == 0);
    REQUIRE(p.ncmds == 1);
    CHECK(argv_is(&p.cmds[0], ARGV("echo", "ab cde")));
    pipeline_free(&p);
}

TEST(quoted_operators_are_literal)
{
    Pipeline p;
    REQUIRE(parse_line("echo \"a|b\" '>' \"&\" '<'", &p) == 0);
    REQUIRE(p.ncmds == 1);
    CHECK(argv_is(&p.cmds[0], ARGV("echo", "a|b", ">", "&", "<")));
    CHECK(p.cmds[0].out_file == NULL && p.background == 0);
    pipeline_free(&p);
}

TEST(pipeline_stages)
{
    Pipeline p;
    REQUIRE(parse_line("cat log.txt | grep -v debug | wc -l", &p) == 0);
    REQUIRE(p.ncmds == 3);
    CHECK(argv_is(&p.cmds[0], ARGV("cat", "log.txt")));
    CHECK(argv_is(&p.cmds[1], ARGV("grep", "-v", "debug")));
    CHECK(argv_is(&p.cmds[2], ARGV("wc", "-l")));
    pipeline_free(&p);
}

TEST(redirections)
{
    Pipeline p;
    REQUIRE(parse_line("sort -r < in.txt > out.txt", &p) == 0);
    REQUIRE(p.ncmds == 1);
    CHECK(argv_is(&p.cmds[0], ARGV("sort", "-r")));
    CHECK(str_is(p.cmds[0].in_file, "in.txt"));
    CHECK(str_is(p.cmds[0].out_file, "out.txt"));
    CHECK(p.cmds[0].append == 0);
    pipeline_free(&p);

    REQUIRE(parse_line("echo done >> build.log", &p) == 0);
    CHECK(str_is(p.cmds[0].out_file, "build.log"));
    CHECK(p.cmds[0].append == 1);
    pipeline_free(&p);
}

TEST(operators_need_no_spaces)
{
    Pipeline p;
    REQUIRE(parse_line("cat<in.txt|tr a-z A-Z>>out.txt&", &p) == 0);
    REQUIRE(p.ncmds == 2);
    CHECK(argv_is(&p.cmds[0], ARGV("cat")));
    CHECK(str_is(p.cmds[0].in_file, "in.txt"));
    CHECK(argv_is(&p.cmds[1], ARGV("tr", "a-z", "A-Z")));
    CHECK(str_is(p.cmds[1].out_file, "out.txt") && p.cmds[1].append == 1);
    CHECK(p.background == 1);
    pipeline_free(&p);
}

TEST(background)
{
    Pipeline p;
    REQUIRE(parse_line("sleep 10 &", &p) == 0);
    REQUIRE(p.ncmds == 1);
    CHECK(argv_is(&p.cmds[0], ARGV("sleep", "10")));
    CHECK(p.background == 1);
    pipeline_free(&p);
}

TEST(syntax_errors)
{
    Pipeline good;
    REQUIRE(parse_line("ls | wc", &good) == 0); /* rejecting everything doesn't count */
    pipeline_free(&good);

    const char *bad[] = {
        "| ls", "ls |", "ls | | wc", "ls >", "ls > | wc", "cat <",
        "echo 'unterminated", "echo \"unterminated", "ls & wc", "&",
    };
    for (size_t i = 0; i < sizeof bad / sizeof *bad; i++) {
        Pipeline p;
        int rc = parse_line(bad[i], &p);
        if (rc != -1)
            fprintf(stderr, "         accepted: %s\n", bad[i]);
        CHECK(rc == -1);
    }
}

int main(void)
{
    RUN(blank_lines);
    RUN(simple_command);
    RUN(quotes);
    RUN(quotes_glue_to_words);
    RUN(quoted_operators_are_literal);
    RUN(pipeline_stages);
    RUN(redirections);
    RUN(operators_need_no_spaces);
    RUN(background);
    RUN(syntax_errors);
    return TEST_REPORT("parse");
}
