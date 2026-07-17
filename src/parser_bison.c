/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30802

/* Bison version string.  */
#define YYBISON_VERSION "3.8.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 1

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1


/* Substitute the variable and function names.  */
#define yyparse         nft_parse
#define yylex           nft_lex
#define yyerror         nft_error
#define yydebug         nft_debug
#define yynerrs         nft_nerrs

/* First part of user prologue.  */
#line 11 "src/parser_bison.y"

#include <nft.h>

#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <inttypes.h>
#include <syslog.h>
#include <net/if.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/if_ether.h>
#include <linux/netfilter.h>
#include <linux/netfilter/nf_tables.h>
#include <linux/netfilter/nf_conntrack_tuple_common.h>
#include <linux/netfilter/nf_nat.h>
#include <linux/netfilter/nf_log.h>
#include <linux/netfilter/nfnetlink_osf.h>
#include <linux/netfilter/nf_synproxy.h>
#include <linux/xfrm.h>
#include <netinet/ip_icmp.h>
#include <netinet/icmp6.h>
#include <libnftnl/common.h>
#include <libnftnl/set.h>
#include <libnftnl/udata.h>

#include <rule.h>
#include <cmd.h>
#include <statement.h>
#include <expression.h>
#include <headers.h>
#include <utils.h>
#include <parser.h>
#include <erec.h>
#include <sctp_chunk.h>

#include "parser_bison.h"

void parser_init(struct nft_ctx *nft, struct parser_state *state,
		 struct list_head *msgs, struct list_head *cmds,
		 struct scope *top_scope)
{
	memset(state, 0, sizeof(*state));
	state->msgs = msgs;
	state->cmds = cmds;
	state->scopes[0] = scope_init(top_scope, NULL);
	init_list_head(&state->indesc_list);
}

static void yyerror(struct location *loc, struct nft_ctx *nft, void *scanner,
		    struct parser_state *state, const char *s)
{
	erec_queue(error(loc, "%s", s), state->msgs);
}

static struct scope *current_scope(const struct parser_state *state)
{
	return state->scopes[state->scope];
}

static int open_scope(struct parser_state *state, struct scope *scope)
{
	if (state->scope >= array_size(state->scopes) - 1) {
		state->scope_err = true;
		return -1;
	}

	scope_init(scope, current_scope(state));
	state->scopes[++state->scope] = scope;

	return 0;
}

static void close_scope(struct parser_state *state)
{
	if (state->scope_err || state->scope == 0) {
		state->scope_err = false;
		return;
	}

	state->scope--;
}

static void location_init(void *scanner, struct parser_state *state,
			  struct location *loc)
{
	memset(loc, 0, sizeof(*loc));
	loc->indesc = state->indesc;
}

static void location_update(struct location *loc, struct location *rhs, int n)
{
	if (n) {
		loc->indesc       = rhs[n].indesc;
		loc->line_offset  = rhs[1].line_offset;
		loc->first_line   = rhs[1].first_line;
		loc->first_column = rhs[1].first_column;
		loc->last_column  = rhs[n].last_column;
	} else {
		loc->indesc       = rhs[0].indesc;
		loc->line_offset  = rhs[0].line_offset;
		loc->first_line   = rhs[0].first_line;
		loc->first_column = loc->last_column = rhs[0].last_column;
	}
}

static struct expr *handle_concat_expr(const struct location *loc,
					 struct expr *expr,
					 struct expr *expr_l, struct expr *expr_r,
					 struct location loc_rhs[3])
{
	if (expr->etype != EXPR_CONCAT) {
		expr = concat_expr_alloc(loc);
		compound_expr_add(expr, expr_l);
	} else {
		location_update(&expr_r->location, loc_rhs, 2);

		expr = expr_l;
		expr->location = *loc;
	}

	compound_expr_add(expr, expr_r);
	return expr;
}

static bool already_set(const void *attr, const struct location *loc,
			struct parser_state *state)
{
	if (!attr)
		return false;

	erec_queue(error(loc, "You can only specify this once. This statement is duplicated."),
		   state->msgs);
	return true;
}

static struct expr *ifname_expr_alloc(const struct location *location,
				      struct list_head *queue,
				      const char *name)
{
	size_t length = strlen(name);
	struct expr *expr;

	if (length == 0) {
		free_const(name);
		erec_queue(error(location, "empty interface name"), queue);
		return NULL;
	}

	if (length >= IFNAMSIZ) {
		free_const(name);
		erec_queue(error(location, "interface name too long"), queue);
		return NULL;
	}

	expr = constant_expr_alloc(location, &ifname_type, BYTEORDER_HOST_ENDIAN,
				   length * BITS_PER_BYTE, name);

	free_const(name);

	return expr;
}

static void timeout_state_free(struct timeout_state *s)
{
	free_const(s->timeout_str);
	free(s);
}

static void timeout_states_free(struct list_head *list)
{
	struct timeout_state *ts, *next;

	list_for_each_entry_safe(ts, next, list, head) {
		list_del(&ts->head);
		timeout_state_free(ts);
	}

	free(list);
}

#define YYLLOC_DEFAULT(Current, Rhs, N)	location_update(&Current, Rhs, N)

#define symbol_value(loc, str) \
	symbol_expr_alloc(loc, SYMBOL_VALUE, current_scope(state), str)

/* Declare those here to avoid compiler warnings */
void nft_set_debug(int, void *);
int nft_lex(void *, void *, void *);

#line 267 "src/parser_bison.c"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

/* Use api.header.include to #include this header
   instead of duplicating it here.  */
#ifndef YY_NFT_SRC_PARSER_BISON_H_INCLUDED
# define YY_NFT_SRC_PARSER_BISON_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 1
#endif
#if YYDEBUG
extern int nft_debug;
#endif

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    TOKEN_EOF = 0,                 /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    JUNK = 258,                    /* "junk"  */
    CRLF = 259,                    /* "CRLF line terminators"  */
    NEWLINE = 260,                 /* "newline"  */
    COLON = 261,                   /* "colon"  */
    SEMICOLON = 262,               /* "semicolon"  */
    COMMA = 263,                   /* "comma"  */
    DOT = 264,                     /* "."  */
    EQ = 265,                      /* "=="  */
    NEQ = 266,                     /* "!="  */
    LT = 267,                      /* "<"  */
    GT = 268,                      /* ">"  */
    GTE = 269,                     /* ">="  */
    LTE = 270,                     /* "<="  */
    LSHIFT = 271,                  /* "<<"  */
    RSHIFT = 272,                  /* ">>"  */
    AMPERSAND = 273,               /* "&"  */
    CARET = 274,                   /* "^"  */
    NOT = 275,                     /* "!"  */
    SLASH = 276,                   /* "/"  */
    ASTERISK = 277,                /* "*"  */
    DASH = 278,                    /* "-"  */
    AT = 279,                      /* "@"  */
    VMAP = 280,                    /* "vmap"  */
    PLUS = 281,                    /* "+"  */
    INCLUDE = 282,                 /* "include"  */
    DEFINE = 283,                  /* "define"  */
    REDEFINE = 284,                /* "redefine"  */
    UNDEFINE = 285,                /* "undefine"  */
    FIB = 286,                     /* "fib"  */
    SOCKET = 287,                  /* "socket"  */
    TRANSPARENT = 288,             /* "transparent"  */
    WILDCARD = 289,                /* "wildcard"  */
    CGROUPV2 = 290,                /* "cgroupv2"  */
    TPROXY = 291,                  /* "tproxy"  */
    OSF = 292,                     /* "osf"  */
    SYNPROXY = 293,                /* "synproxy"  */
    MSS = 294,                     /* "mss"  */
    WSCALE = 295,                  /* "wscale"  */
    TYPEOF = 296,                  /* "typeof"  */
    HOOK = 297,                    /* "hook"  */
    HOOKS = 298,                   /* "hooks"  */
    DEVICE = 299,                  /* "device"  */
    DEVICES = 300,                 /* "devices"  */
    TABLE = 301,                   /* "table"  */
    TABLES = 302,                  /* "tables"  */
    CHAIN = 303,                   /* "chain"  */
    CHAINS = 304,                  /* "chains"  */
    RULE = 305,                    /* "rule"  */
    RULES = 306,                   /* "rules"  */
    SETS = 307,                    /* "sets"  */
    SET = 308,                     /* "set"  */
    ELEMENT = 309,                 /* "element"  */
    MAP = 310,                     /* "map"  */
    MAPS = 311,                    /* "maps"  */
    FLOWTABLE = 312,               /* "flowtable"  */
    HANDLE = 313,                  /* "handle"  */
    RULESET = 314,                 /* "ruleset"  */
    TRACE = 315,                   /* "trace"  */
    INET = 316,                    /* "inet"  */
    NETDEV = 317,                  /* "netdev"  */
    ADD = 318,                     /* "add"  */
    UPDATE = 319,                  /* "update"  */
    REPLACE = 320,                 /* "replace"  */
    CREATE = 321,                  /* "create"  */
    INSERT = 322,                  /* "insert"  */
    DELETE = 323,                  /* "delete"  */
    GET = 324,                     /* "get"  */
    LIST = 325,                    /* "list"  */
    RESET = 326,                   /* "reset"  */
    FLUSH = 327,                   /* "flush"  */
    RENAME = 328,                  /* "rename"  */
    DESCRIBE = 329,                /* "describe"  */
    IMPORT = 330,                  /* "import"  */
    EXPORT = 331,                  /* "export"  */
    DESTROY = 332,                 /* "destroy"  */
    MONITOR = 333,                 /* "monitor"  */
    ALL = 334,                     /* "all"  */
    ACCEPT = 335,                  /* "accept"  */
    DROP = 336,                    /* "drop"  */
    CONTINUE = 337,                /* "continue"  */
    JUMP = 338,                    /* "jump"  */
    GOTO = 339,                    /* "goto"  */
    RETURN = 340,                  /* "return"  */
    TO = 341,                      /* "to"  */
    CONSTANT = 342,                /* "constant"  */
    INTERVAL = 343,                /* "interval"  */
    DYNAMIC = 344,                 /* "dynamic"  */
    AUTOMERGE = 345,               /* "auto-merge"  */
    TIMEOUT = 346,                 /* "timeout"  */
    GC_INTERVAL = 347,             /* "gc-interval"  */
    ELEMENTS = 348,                /* "elements"  */
    EXPIRES = 349,                 /* "expires"  */
    POLICY = 350,                  /* "policy"  */
    MEMORY = 351,                  /* "memory"  */
    PERFORMANCE = 352,             /* "performance"  */
    SIZE = 353,                    /* "size"  */
    FLOW = 354,                    /* "flow"  */
    OFFLOAD = 355,                 /* "offload"  */
    METER = 356,                   /* "meter"  */
    METERS = 357,                  /* "meters"  */
    FLOWTABLES = 358,              /* "flowtables"  */
    NUM = 359,                     /* "number"  */
    STRING = 360,                  /* "string"  */
    QUOTED_STRING = 361,           /* "quoted string"  */
    ASTERISK_STRING = 362,         /* "string with a trailing asterisk"  */
    LL_HDR = 363,                  /* "ll"  */
    NETWORK_HDR = 364,             /* "nh"  */
    TRANSPORT_HDR = 365,           /* "th"  */
    BRIDGE = 366,                  /* "bridge"  */
    ETHER = 367,                   /* "ether"  */
    SADDR = 368,                   /* "saddr"  */
    DADDR = 369,                   /* "daddr"  */
    TYPE = 370,                    /* "type"  */
    VLAN = 371,                    /* "vlan"  */
    ID = 372,                      /* "id"  */
    CFI = 373,                     /* "cfi"  */
    DEI = 374,                     /* "dei"  */
    PCP = 375,                     /* "pcp"  */
    ARP = 376,                     /* "arp"  */
    HTYPE = 377,                   /* "htype"  */
    PTYPE = 378,                   /* "ptype"  */
    HLEN = 379,                    /* "hlen"  */
    PLEN = 380,                    /* "plen"  */
    OPERATION = 381,               /* "operation"  */
    IP = 382,                      /* "ip"  */
    HDRVERSION = 383,              /* "version"  */
    HDRLENGTH = 384,               /* "hdrlength"  */
    DSCP = 385,                    /* "dscp"  */
    ECN = 386,                     /* "ecn"  */
    LENGTH = 387,                  /* "length"  */
    FRAG_OFF = 388,                /* "frag-off"  */
    TTL = 389,                     /* "ttl"  */
    PROTOCOL = 390,                /* "protocol"  */
    CHECKSUM = 391,                /* "checksum"  */
    PTR = 392,                     /* "ptr"  */
    VALUE = 393,                   /* "value"  */
    LSRR = 394,                    /* "lsrr"  */
    RR = 395,                      /* "rr"  */
    SSRR = 396,                    /* "ssrr"  */
    RA = 397,                      /* "ra"  */
    ICMP = 398,                    /* "icmp"  */
    CODE = 399,                    /* "code"  */
    SEQUENCE = 400,                /* "seq"  */
    GATEWAY = 401,                 /* "gateway"  */
    MTU = 402,                     /* "mtu"  */
    IGMP = 403,                    /* "igmp"  */
    MRT = 404,                     /* "mrt"  */
    OPTIONS = 405,                 /* "options"  */
    IP6 = 406,                     /* "ip6"  */
    PRIORITY = 407,                /* "priority"  */
    FLOWLABEL = 408,               /* "flowlabel"  */
    NEXTHDR = 409,                 /* "nexthdr"  */
    HOPLIMIT = 410,                /* "hoplimit"  */
    ICMP6 = 411,                   /* "icmpv6"  */
    PPTR = 412,                    /* "param-problem"  */
    MAXDELAY = 413,                /* "max-delay"  */
    TADDR = 414,                   /* "taddr"  */
    AH = 415,                      /* "ah"  */
    RESERVED = 416,                /* "reserved"  */
    SPI = 417,                     /* "spi"  */
    ESP = 418,                     /* "esp"  */
    COMP = 419,                    /* "comp"  */
    FLAGS = 420,                   /* "flags"  */
    CPI = 421,                     /* "cpi"  */
    PORT = 422,                    /* "port"  */
    UDP = 423,                     /* "udp"  */
    SPORT = 424,                   /* "sport"  */
    DPORT = 425,                   /* "dport"  */
    UDPLITE = 426,                 /* "udplite"  */
    CSUMCOV = 427,                 /* "csumcov"  */
    TCP = 428,                     /* "tcp"  */
    ACKSEQ = 429,                  /* "ackseq"  */
    DOFF = 430,                    /* "doff"  */
    WINDOW = 431,                  /* "window"  */
    URGPTR = 432,                  /* "urgptr"  */
    OPTION = 433,                  /* "option"  */
    ECHO = 434,                    /* "echo"  */
    EOL = 435,                     /* "eol"  */
    MPTCP = 436,                   /* "mptcp"  */
    NOP = 437,                     /* "nop"  */
    SACK = 438,                    /* "sack"  */
    SACK0 = 439,                   /* "sack0"  */
    SACK1 = 440,                   /* "sack1"  */
    SACK2 = 441,                   /* "sack2"  */
    SACK3 = 442,                   /* "sack3"  */
    SACK_PERM = 443,               /* "sack-permitted"  */
    FASTOPEN = 444,                /* "fastopen"  */
    MD5SIG = 445,                  /* "md5sig"  */
    TIMESTAMP = 446,               /* "timestamp"  */
    COUNT = 447,                   /* "count"  */
    LEFT = 448,                    /* "left"  */
    RIGHT = 449,                   /* "right"  */
    TSVAL = 450,                   /* "tsval"  */
    TSECR = 451,                   /* "tsecr"  */
    SUBTYPE = 452,                 /* "subtype"  */
    DCCP = 453,                    /* "dccp"  */
    VXLAN = 454,                   /* "vxlan"  */
    VNI = 455,                     /* "vni"  */
    GRE = 456,                     /* "gre"  */
    GRETAP = 457,                  /* "gretap"  */
    GENEVE = 458,                  /* "geneve"  */
    SCTP = 459,                    /* "sctp"  */
    CHUNK = 460,                   /* "chunk"  */
    DATA = 461,                    /* "data"  */
    INIT = 462,                    /* "init"  */
    INIT_ACK = 463,                /* "init-ack"  */
    HEARTBEAT = 464,               /* "heartbeat"  */
    HEARTBEAT_ACK = 465,           /* "heartbeat-ack"  */
    ABORT = 466,                   /* "abort"  */
    SHUTDOWN = 467,                /* "shutdown"  */
    SHUTDOWN_ACK = 468,            /* "shutdown-ack"  */
    ERROR = 469,                   /* "error"  */
    COOKIE_ECHO = 470,             /* "cookie-echo"  */
    COOKIE_ACK = 471,              /* "cookie-ack"  */
    ECNE = 472,                    /* "ecne"  */
    CWR = 473,                     /* "cwr"  */
    SHUTDOWN_COMPLETE = 474,       /* "shutdown-complete"  */
    ASCONF_ACK = 475,              /* "asconf-ack"  */
    FORWARD_TSN = 476,             /* "forward-tsn"  */
    ASCONF = 477,                  /* "asconf"  */
    TSN = 478,                     /* "tsn"  */
    STREAM = 479,                  /* "stream"  */
    SSN = 480,                     /* "ssn"  */
    PPID = 481,                    /* "ppid"  */
    INIT_TAG = 482,                /* "init-tag"  */
    A_RWND = 483,                  /* "a-rwnd"  */
    NUM_OSTREAMS = 484,            /* "num-outbound-streams"  */
    NUM_ISTREAMS = 485,            /* "num-inbound-streams"  */
    INIT_TSN = 486,                /* "initial-tsn"  */
    CUM_TSN_ACK = 487,             /* "cum-tsn-ack"  */
    NUM_GACK_BLOCKS = 488,         /* "num-gap-ack-blocks"  */
    NUM_DUP_TSNS = 489,            /* "num-dup-tsns"  */
    LOWEST_TSN = 490,              /* "lowest-tsn"  */
    SEQNO = 491,                   /* "seqno"  */
    NEW_CUM_TSN = 492,             /* "new-cum-tsn"  */
    VTAG = 493,                    /* "vtag"  */
    RT = 494,                      /* "rt"  */
    RT0 = 495,                     /* "rt0"  */
    RT2 = 496,                     /* "rt2"  */
    RT4 = 497,                     /* "srh"  */
    SEG_LEFT = 498,                /* "seg-left"  */
    ADDR = 499,                    /* "addr"  */
    LAST_ENT = 500,                /* "last-entry"  */
    TAG = 501,                     /* "tag"  */
    SID = 502,                     /* "sid"  */
    HBH = 503,                     /* "hbh"  */
    FRAG = 504,                    /* "frag"  */
    RESERVED2 = 505,               /* "reserved2"  */
    MORE_FRAGMENTS = 506,          /* "more-fragments"  */
    DST = 507,                     /* "dst"  */
    MH = 508,                      /* "mh"  */
    META = 509,                    /* "meta"  */
    MARK = 510,                    /* "mark"  */
    IIF = 511,                     /* "iif"  */
    IIFNAME = 512,                 /* "iifname"  */
    IIFTYPE = 513,                 /* "iiftype"  */
    OIF = 514,                     /* "oif"  */
    OIFNAME = 515,                 /* "oifname"  */
    OIFTYPE = 516,                 /* "oiftype"  */
    SKUID = 517,                   /* "skuid"  */
    SKGID = 518,                   /* "skgid"  */
    NFTRACE = 519,                 /* "nftrace"  */
    RTCLASSID = 520,               /* "rtclassid"  */
    IBRIPORT = 521,                /* "ibriport"  */
    OBRIPORT = 522,                /* "obriport"  */
    IBRIDGENAME = 523,             /* "ibrname"  */
    OBRIDGENAME = 524,             /* "obrname"  */
    PKTTYPE = 525,                 /* "pkttype"  */
    CPU = 526,                     /* "cpu"  */
    IIFGROUP = 527,                /* "iifgroup"  */
    OIFGROUP = 528,                /* "oifgroup"  */
    CGROUP = 529,                  /* "cgroup"  */
    TIME = 530,                    /* "time"  */
    CLASSID = 531,                 /* "classid"  */
    NEXTHOP = 532,                 /* "nexthop"  */
    CT = 533,                      /* "ct"  */
    L3PROTOCOL = 534,              /* "l3proto"  */
    PROTO_SRC = 535,               /* "proto-src"  */
    PROTO_DST = 536,               /* "proto-dst"  */
    ZONE = 537,                    /* "zone"  */
    DIRECTION = 538,               /* "direction"  */
    EVENT = 539,                   /* "event"  */
    EXPECTATION = 540,             /* "expectation"  */
    EXPIRATION = 541,              /* "expiration"  */
    HELPER = 542,                  /* "helper"  */
    LABEL = 543,                   /* "label"  */
    STATE = 544,                   /* "state"  */
    STATUS = 545,                  /* "status"  */
    ORIGINAL = 546,                /* "original"  */
    REPLY = 547,                   /* "reply"  */
    COUNTER = 548,                 /* "counter"  */
    NAME = 549,                    /* "name"  */
    PACKETS = 550,                 /* "packets"  */
    BYTES = 551,                   /* "bytes"  */
    AVGPKT = 552,                  /* "avgpkt"  */
    LAST = 553,                    /* "last"  */
    NEVER = 554,                   /* "never"  */
    COUNTERS = 555,                /* "counters"  */
    QUOTAS = 556,                  /* "quotas"  */
    LIMITS = 557,                  /* "limits"  */
    SYNPROXYS = 558,               /* "synproxys"  */
    HELPERS = 559,                 /* "helpers"  */
    LOG = 560,                     /* "log"  */
    PREFIX = 561,                  /* "prefix"  */
    GROUP = 562,                   /* "group"  */
    SNAPLEN = 563,                 /* "snaplen"  */
    QUEUE_THRESHOLD = 564,         /* "queue-threshold"  */
    LEVEL = 565,                   /* "level"  */
    LIMIT = 566,                   /* "limit"  */
    RATE = 567,                    /* "rate"  */
    BURST = 568,                   /* "burst"  */
    OVER = 569,                    /* "over"  */
    UNTIL = 570,                   /* "until"  */
    QUOTA = 571,                   /* "quota"  */
    USED = 572,                    /* "used"  */
    SECMARK = 573,                 /* "secmark"  */
    SECMARKS = 574,                /* "secmarks"  */
    SECOND = 575,                  /* "second"  */
    MINUTE = 576,                  /* "minute"  */
    HOUR = 577,                    /* "hour"  */
    DAY = 578,                     /* "day"  */
    WEEK = 579,                    /* "week"  */
    _REJECT = 580,                 /* "reject"  */
    WITH = 581,                    /* "with"  */
    ICMPX = 582,                   /* "icmpx"  */
    SNAT = 583,                    /* "snat"  */
    DNAT = 584,                    /* "dnat"  */
    MASQUERADE = 585,              /* "masquerade"  */
    REDIRECT = 586,                /* "redirect"  */
    RANDOM = 587,                  /* "random"  */
    FULLY_RANDOM = 588,            /* "fully-random"  */
    PERSISTENT = 589,              /* "persistent"  */
    QUEUE = 590,                   /* "queue"  */
    QUEUENUM = 591,                /* "num"  */
    BYPASS = 592,                  /* "bypass"  */
    FANOUT = 593,                  /* "fanout"  */
    DUP = 594,                     /* "dup"  */
    FWD = 595,                     /* "fwd"  */
    NUMGEN = 596,                  /* "numgen"  */
    INC = 597,                     /* "inc"  */
    MOD = 598,                     /* "mod"  */
    OFFSET = 599,                  /* "offset"  */
    JHASH = 600,                   /* "jhash"  */
    SYMHASH = 601,                 /* "symhash"  */
    SEED = 602,                    /* "seed"  */
    POSITION = 603,                /* "position"  */
    INDEX = 604,                   /* "index"  */
    COMMENT = 605,                 /* "comment"  */
    XML = 606,                     /* "xml"  */
    JSON = 607,                    /* "json"  */
    VM = 608,                      /* "vm"  */
    NOTRACK = 609,                 /* "notrack"  */
    EXISTS = 610,                  /* "exists"  */
    MISSING = 611,                 /* "missing"  */
    EXTHDR = 612,                  /* "exthdr"  */
    IPSEC = 613,                   /* "ipsec"  */
    REQID = 614,                   /* "reqid"  */
    SPNUM = 615,                   /* "spnum"  */
    IN = 616,                      /* "in"  */
    OUT = 617,                     /* "out"  */
    XT = 618                       /* "xt"  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif
/* Token kinds.  */
#define YYEMPTY -2
#define TOKEN_EOF 0
#define YYerror 256
#define YYUNDEF 257
#define JUNK 258
#define CRLF 259
#define NEWLINE 260
#define COLON 261
#define SEMICOLON 262
#define COMMA 263
#define DOT 264
#define EQ 265
#define NEQ 266
#define LT 267
#define GT 268
#define GTE 269
#define LTE 270
#define LSHIFT 271
#define RSHIFT 272
#define AMPERSAND 273
#define CARET 274
#define NOT 275
#define SLASH 276
#define ASTERISK 277
#define DASH 278
#define AT 279
#define VMAP 280
#define PLUS 281
#define INCLUDE 282
#define DEFINE 283
#define REDEFINE 284
#define UNDEFINE 285
#define FIB 286
#define SOCKET 287
#define TRANSPARENT 288
#define WILDCARD 289
#define CGROUPV2 290
#define TPROXY 291
#define OSF 292
#define SYNPROXY 293
#define MSS 294
#define WSCALE 295
#define TYPEOF 296
#define HOOK 297
#define HOOKS 298
#define DEVICE 299
#define DEVICES 300
#define TABLE 301
#define TABLES 302
#define CHAIN 303
#define CHAINS 304
#define RULE 305
#define RULES 306
#define SETS 307
#define SET 308
#define ELEMENT 309
#define MAP 310
#define MAPS 311
#define FLOWTABLE 312
#define HANDLE 313
#define RULESET 314
#define TRACE 315
#define INET 316
#define NETDEV 317
#define ADD 318
#define UPDATE 319
#define REPLACE 320
#define CREATE 321
#define INSERT 322
#define DELETE 323
#define GET 324
#define LIST 325
#define RESET 326
#define FLUSH 327
#define RENAME 328
#define DESCRIBE 329
#define IMPORT 330
#define EXPORT 331
#define DESTROY 332
#define MONITOR 333
#define ALL 334
#define ACCEPT 335
#define DROP 336
#define CONTINUE 337
#define JUMP 338
#define GOTO 339
#define RETURN 340
#define TO 341
#define CONSTANT 342
#define INTERVAL 343
#define DYNAMIC 344
#define AUTOMERGE 345
#define TIMEOUT 346
#define GC_INTERVAL 347
#define ELEMENTS 348
#define EXPIRES 349
#define POLICY 350
#define MEMORY 351
#define PERFORMANCE 352
#define SIZE 353
#define FLOW 354
#define OFFLOAD 355
#define METER 356
#define METERS 357
#define FLOWTABLES 358
#define NUM 359
#define STRING 360
#define QUOTED_STRING 361
#define ASTERISK_STRING 362
#define LL_HDR 363
#define NETWORK_HDR 364
#define TRANSPORT_HDR 365
#define BRIDGE 366
#define ETHER 367
#define SADDR 368
#define DADDR 369
#define TYPE 370
#define VLAN 371
#define ID 372
#define CFI 373
#define DEI 374
#define PCP 375
#define ARP 376
#define HTYPE 377
#define PTYPE 378
#define HLEN 379
#define PLEN 380
#define OPERATION 381
#define IP 382
#define HDRVERSION 383
#define HDRLENGTH 384
#define DSCP 385
#define ECN 386
#define LENGTH 387
#define FRAG_OFF 388
#define TTL 389
#define PROTOCOL 390
#define CHECKSUM 391
#define PTR 392
#define VALUE 393
#define LSRR 394
#define RR 395
#define SSRR 396
#define RA 397
#define ICMP 398
#define CODE 399
#define SEQUENCE 400
#define GATEWAY 401
#define MTU 402
#define IGMP 403
#define MRT 404
#define OPTIONS 405
#define IP6 406
#define PRIORITY 407
#define FLOWLABEL 408
#define NEXTHDR 409
#define HOPLIMIT 410
#define ICMP6 411
#define PPTR 412
#define MAXDELAY 413
#define TADDR 414
#define AH 415
#define RESERVED 416
#define SPI 417
#define ESP 418
#define COMP 419
#define FLAGS 420
#define CPI 421
#define PORT 422
#define UDP 423
#define SPORT 424
#define DPORT 425
#define UDPLITE 426
#define CSUMCOV 427
#define TCP 428
#define ACKSEQ 429
#define DOFF 430
#define WINDOW 431
#define URGPTR 432
#define OPTION 433
#define ECHO 434
#define EOL 435
#define MPTCP 436
#define NOP 437
#define SACK 438
#define SACK0 439
#define SACK1 440
#define SACK2 441
#define SACK3 442
#define SACK_PERM 443
#define FASTOPEN 444
#define MD5SIG 445
#define TIMESTAMP 446
#define COUNT 447
#define LEFT 448
#define RIGHT 449
#define TSVAL 450
#define TSECR 451
#define SUBTYPE 452
#define DCCP 453
#define VXLAN 454
#define VNI 455
#define GRE 456
#define GRETAP 457
#define GENEVE 458
#define SCTP 459
#define CHUNK 460
#define DATA 461
#define INIT 462
#define INIT_ACK 463
#define HEARTBEAT 464
#define HEARTBEAT_ACK 465
#define ABORT 466
#define SHUTDOWN 467
#define SHUTDOWN_ACK 468
#define ERROR 469
#define COOKIE_ECHO 470
#define COOKIE_ACK 471
#define ECNE 472
#define CWR 473
#define SHUTDOWN_COMPLETE 474
#define ASCONF_ACK 475
#define FORWARD_TSN 476
#define ASCONF 477
#define TSN 478
#define STREAM 479
#define SSN 480
#define PPID 481
#define INIT_TAG 482
#define A_RWND 483
#define NUM_OSTREAMS 484
#define NUM_ISTREAMS 485
#define INIT_TSN 486
#define CUM_TSN_ACK 487
#define NUM_GACK_BLOCKS 488
#define NUM_DUP_TSNS 489
#define LOWEST_TSN 490
#define SEQNO 491
#define NEW_CUM_TSN 492
#define VTAG 493
#define RT 494
#define RT0 495
#define RT2 496
#define RT4 497
#define SEG_LEFT 498
#define ADDR 499
#define LAST_ENT 500
#define TAG 501
#define SID 502
#define HBH 503
#define FRAG 504
#define RESERVED2 505
#define MORE_FRAGMENTS 506
#define DST 507
#define MH 508
#define META 509
#define MARK 510
#define IIF 511
#define IIFNAME 512
#define IIFTYPE 513
#define OIF 514
#define OIFNAME 515
#define OIFTYPE 516
#define SKUID 517
#define SKGID 518
#define NFTRACE 519
#define RTCLASSID 520
#define IBRIPORT 521
#define OBRIPORT 522
#define IBRIDGENAME 523
#define OBRIDGENAME 524
#define PKTTYPE 525
#define CPU 526
#define IIFGROUP 527
#define OIFGROUP 528
#define CGROUP 529
#define TIME 530
#define CLASSID 531
#define NEXTHOP 532
#define CT 533
#define L3PROTOCOL 534
#define PROTO_SRC 535
#define PROTO_DST 536
#define ZONE 537
#define DIRECTION 538
#define EVENT 539
#define EXPECTATION 540
#define EXPIRATION 541
#define HELPER 542
#define LABEL 543
#define STATE 544
#define STATUS 545
#define ORIGINAL 546
#define REPLY 547
#define COUNTER 548
#define NAME 549
#define PACKETS 550
#define BYTES 551
#define AVGPKT 552
#define LAST 553
#define NEVER 554
#define COUNTERS 555
#define QUOTAS 556
#define LIMITS 557
#define SYNPROXYS 558
#define HELPERS 559
#define LOG 560
#define PREFIX 561
#define GROUP 562
#define SNAPLEN 563
#define QUEUE_THRESHOLD 564
#define LEVEL 565
#define LIMIT 566
#define RATE 567
#define BURST 568
#define OVER 569
#define UNTIL 570
#define QUOTA 571
#define USED 572
#define SECMARK 573
#define SECMARKS 574
#define SECOND 575
#define MINUTE 576
#define HOUR 577
#define DAY 578
#define WEEK 579
#define _REJECT 580
#define WITH 581
#define ICMPX 582
#define SNAT 583
#define DNAT 584
#define MASQUERADE 585
#define REDIRECT 586
#define RANDOM 587
#define FULLY_RANDOM 588
#define PERSISTENT 589
#define QUEUE 590
#define QUEUENUM 591
#define BYPASS 592
#define FANOUT 593
#define DUP 594
#define FWD 595
#define NUMGEN 596
#define INC 597
#define MOD 598
#define OFFSET 599
#define JHASH 600
#define SYMHASH 601
#define SEED 602
#define POSITION 603
#define INDEX 604
#define COMMENT 605
#define XML 606
#define JSON 607
#define VM 608
#define NOTRACK 609
#define EXISTS 610
#define MISSING 611
#define EXTHDR 612
#define IPSEC 613
#define REQID 614
#define SPNUM 615
#define IN 616
#define OUT 617
#define XT 618

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 222 "src/parser_bison.y"

	uint64_t		val;
	uint32_t		val32;
	uint8_t			val8;
	const char *		string;

	struct list_head	*list;
	struct cmd		*cmd;
	struct handle		handle;
	struct table		*table;
	struct chain		*chain;
	struct rule		*rule;
	struct stmt		*stmt;
	struct expr		*expr;
	struct set		*set;
	struct obj		*obj;
	struct flowtable	*flowtable;
	struct ct		*ct;
	const struct datatype	*datatype;
	struct handle_spec	handle_spec;
	struct position_spec	position_spec;
	struct prio_spec	prio_spec;
	struct limit_rate	limit_rate;
	struct tcp_kind_field {
		uint16_t kind; /* must allow > 255 for SACK1, 2.. hack */
		uint8_t field;
	} tcp_kind_field;
	struct timeout_state	*timeout_state;

#line 1076 "src/parser_bison.c"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif

/* Location type.  */
#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE YYLTYPE;
struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
};
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif




int nft_parse (struct nft_ctx *nft, void *scanner, struct parser_state *state);


#endif /* !YY_NFT_SRC_PARSER_BISON_H_INCLUDED  */
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_JUNK = 3,                       /* "junk"  */
  YYSYMBOL_CRLF = 4,                       /* "CRLF line terminators"  */
  YYSYMBOL_NEWLINE = 5,                    /* "newline"  */
  YYSYMBOL_COLON = 6,                      /* "colon"  */
  YYSYMBOL_SEMICOLON = 7,                  /* "semicolon"  */
  YYSYMBOL_COMMA = 8,                      /* "comma"  */
  YYSYMBOL_DOT = 9,                        /* "."  */
  YYSYMBOL_EQ = 10,                        /* "=="  */
  YYSYMBOL_NEQ = 11,                       /* "!="  */
  YYSYMBOL_LT = 12,                        /* "<"  */
  YYSYMBOL_GT = 13,                        /* ">"  */
  YYSYMBOL_GTE = 14,                       /* ">="  */
  YYSYMBOL_LTE = 15,                       /* "<="  */
  YYSYMBOL_LSHIFT = 16,                    /* "<<"  */
  YYSYMBOL_RSHIFT = 17,                    /* ">>"  */
  YYSYMBOL_AMPERSAND = 18,                 /* "&"  */
  YYSYMBOL_CARET = 19,                     /* "^"  */
  YYSYMBOL_NOT = 20,                       /* "!"  */
  YYSYMBOL_SLASH = 21,                     /* "/"  */
  YYSYMBOL_ASTERISK = 22,                  /* "*"  */
  YYSYMBOL_DASH = 23,                      /* "-"  */
  YYSYMBOL_AT = 24,                        /* "@"  */
  YYSYMBOL_VMAP = 25,                      /* "vmap"  */
  YYSYMBOL_PLUS = 26,                      /* "+"  */
  YYSYMBOL_INCLUDE = 27,                   /* "include"  */
  YYSYMBOL_DEFINE = 28,                    /* "define"  */
  YYSYMBOL_REDEFINE = 29,                  /* "redefine"  */
  YYSYMBOL_UNDEFINE = 30,                  /* "undefine"  */
  YYSYMBOL_FIB = 31,                       /* "fib"  */
  YYSYMBOL_SOCKET = 32,                    /* "socket"  */
  YYSYMBOL_TRANSPARENT = 33,               /* "transparent"  */
  YYSYMBOL_WILDCARD = 34,                  /* "wildcard"  */
  YYSYMBOL_CGROUPV2 = 35,                  /* "cgroupv2"  */
  YYSYMBOL_TPROXY = 36,                    /* "tproxy"  */
  YYSYMBOL_OSF = 37,                       /* "osf"  */
  YYSYMBOL_SYNPROXY = 38,                  /* "synproxy"  */
  YYSYMBOL_MSS = 39,                       /* "mss"  */
  YYSYMBOL_WSCALE = 40,                    /* "wscale"  */
  YYSYMBOL_TYPEOF = 41,                    /* "typeof"  */
  YYSYMBOL_HOOK = 42,                      /* "hook"  */
  YYSYMBOL_HOOKS = 43,                     /* "hooks"  */
  YYSYMBOL_DEVICE = 44,                    /* "device"  */
  YYSYMBOL_DEVICES = 45,                   /* "devices"  */
  YYSYMBOL_TABLE = 46,                     /* "table"  */
  YYSYMBOL_TABLES = 47,                    /* "tables"  */
  YYSYMBOL_CHAIN = 48,                     /* "chain"  */
  YYSYMBOL_CHAINS = 49,                    /* "chains"  */
  YYSYMBOL_RULE = 50,                      /* "rule"  */
  YYSYMBOL_RULES = 51,                     /* "rules"  */
  YYSYMBOL_SETS = 52,                      /* "sets"  */
  YYSYMBOL_SET = 53,                       /* "set"  */
  YYSYMBOL_ELEMENT = 54,                   /* "element"  */
  YYSYMBOL_MAP = 55,                       /* "map"  */
  YYSYMBOL_MAPS = 56,                      /* "maps"  */
  YYSYMBOL_FLOWTABLE = 57,                 /* "flowtable"  */
  YYSYMBOL_HANDLE = 58,                    /* "handle"  */
  YYSYMBOL_RULESET = 59,                   /* "ruleset"  */
  YYSYMBOL_TRACE = 60,                     /* "trace"  */
  YYSYMBOL_INET = 61,                      /* "inet"  */
  YYSYMBOL_NETDEV = 62,                    /* "netdev"  */
  YYSYMBOL_ADD = 63,                       /* "add"  */
  YYSYMBOL_UPDATE = 64,                    /* "update"  */
  YYSYMBOL_REPLACE = 65,                   /* "replace"  */
  YYSYMBOL_CREATE = 66,                    /* "create"  */
  YYSYMBOL_INSERT = 67,                    /* "insert"  */
  YYSYMBOL_DELETE = 68,                    /* "delete"  */
  YYSYMBOL_GET = 69,                       /* "get"  */
  YYSYMBOL_LIST = 70,                      /* "list"  */
  YYSYMBOL_RESET = 71,                     /* "reset"  */
  YYSYMBOL_FLUSH = 72,                     /* "flush"  */
  YYSYMBOL_RENAME = 73,                    /* "rename"  */
  YYSYMBOL_DESCRIBE = 74,                  /* "describe"  */
  YYSYMBOL_IMPORT = 75,                    /* "import"  */
  YYSYMBOL_EXPORT = 76,                    /* "export"  */
  YYSYMBOL_DESTROY = 77,                   /* "destroy"  */
  YYSYMBOL_MONITOR = 78,                   /* "monitor"  */
  YYSYMBOL_ALL = 79,                       /* "all"  */
  YYSYMBOL_ACCEPT = 80,                    /* "accept"  */
  YYSYMBOL_DROP = 81,                      /* "drop"  */
  YYSYMBOL_CONTINUE = 82,                  /* "continue"  */
  YYSYMBOL_JUMP = 83,                      /* "jump"  */
  YYSYMBOL_GOTO = 84,                      /* "goto"  */
  YYSYMBOL_RETURN = 85,                    /* "return"  */
  YYSYMBOL_TO = 86,                        /* "to"  */
  YYSYMBOL_CONSTANT = 87,                  /* "constant"  */
  YYSYMBOL_INTERVAL = 88,                  /* "interval"  */
  YYSYMBOL_DYNAMIC = 89,                   /* "dynamic"  */
  YYSYMBOL_AUTOMERGE = 90,                 /* "auto-merge"  */
  YYSYMBOL_TIMEOUT = 91,                   /* "timeout"  */
  YYSYMBOL_GC_INTERVAL = 92,               /* "gc-interval"  */
  YYSYMBOL_ELEMENTS = 93,                  /* "elements"  */
  YYSYMBOL_EXPIRES = 94,                   /* "expires"  */
  YYSYMBOL_POLICY = 95,                    /* "policy"  */
  YYSYMBOL_MEMORY = 96,                    /* "memory"  */
  YYSYMBOL_PERFORMANCE = 97,               /* "performance"  */
  YYSYMBOL_SIZE = 98,                      /* "size"  */
  YYSYMBOL_FLOW = 99,                      /* "flow"  */
  YYSYMBOL_OFFLOAD = 100,                  /* "offload"  */
  YYSYMBOL_METER = 101,                    /* "meter"  */
  YYSYMBOL_METERS = 102,                   /* "meters"  */
  YYSYMBOL_FLOWTABLES = 103,               /* "flowtables"  */
  YYSYMBOL_NUM = 104,                      /* "number"  */
  YYSYMBOL_STRING = 105,                   /* "string"  */
  YYSYMBOL_QUOTED_STRING = 106,            /* "quoted string"  */
  YYSYMBOL_ASTERISK_STRING = 107,          /* "string with a trailing asterisk"  */
  YYSYMBOL_LL_HDR = 108,                   /* "ll"  */
  YYSYMBOL_NETWORK_HDR = 109,              /* "nh"  */
  YYSYMBOL_TRANSPORT_HDR = 110,            /* "th"  */
  YYSYMBOL_BRIDGE = 111,                   /* "bridge"  */
  YYSYMBOL_ETHER = 112,                    /* "ether"  */
  YYSYMBOL_SADDR = 113,                    /* "saddr"  */
  YYSYMBOL_DADDR = 114,                    /* "daddr"  */
  YYSYMBOL_TYPE = 115,                     /* "type"  */
  YYSYMBOL_VLAN = 116,                     /* "vlan"  */
  YYSYMBOL_ID = 117,                       /* "id"  */
  YYSYMBOL_CFI = 118,                      /* "cfi"  */
  YYSYMBOL_DEI = 119,                      /* "dei"  */
  YYSYMBOL_PCP = 120,                      /* "pcp"  */
  YYSYMBOL_ARP = 121,                      /* "arp"  */
  YYSYMBOL_HTYPE = 122,                    /* "htype"  */
  YYSYMBOL_PTYPE = 123,                    /* "ptype"  */
  YYSYMBOL_HLEN = 124,                     /* "hlen"  */
  YYSYMBOL_PLEN = 125,                     /* "plen"  */
  YYSYMBOL_OPERATION = 126,                /* "operation"  */
  YYSYMBOL_IP = 127,                       /* "ip"  */
  YYSYMBOL_HDRVERSION = 128,               /* "version"  */
  YYSYMBOL_HDRLENGTH = 129,                /* "hdrlength"  */
  YYSYMBOL_DSCP = 130,                     /* "dscp"  */
  YYSYMBOL_ECN = 131,                      /* "ecn"  */
  YYSYMBOL_LENGTH = 132,                   /* "length"  */
  YYSYMBOL_FRAG_OFF = 133,                 /* "frag-off"  */
  YYSYMBOL_TTL = 134,                      /* "ttl"  */
  YYSYMBOL_PROTOCOL = 135,                 /* "protocol"  */
  YYSYMBOL_CHECKSUM = 136,                 /* "checksum"  */
  YYSYMBOL_PTR = 137,                      /* "ptr"  */
  YYSYMBOL_VALUE = 138,                    /* "value"  */
  YYSYMBOL_LSRR = 139,                     /* "lsrr"  */
  YYSYMBOL_RR = 140,                       /* "rr"  */
  YYSYMBOL_SSRR = 141,                     /* "ssrr"  */
  YYSYMBOL_RA = 142,                       /* "ra"  */
  YYSYMBOL_ICMP = 143,                     /* "icmp"  */
  YYSYMBOL_CODE = 144,                     /* "code"  */
  YYSYMBOL_SEQUENCE = 145,                 /* "seq"  */
  YYSYMBOL_GATEWAY = 146,                  /* "gateway"  */
  YYSYMBOL_MTU = 147,                      /* "mtu"  */
  YYSYMBOL_IGMP = 148,                     /* "igmp"  */
  YYSYMBOL_MRT = 149,                      /* "mrt"  */
  YYSYMBOL_OPTIONS = 150,                  /* "options"  */
  YYSYMBOL_IP6 = 151,                      /* "ip6"  */
  YYSYMBOL_PRIORITY = 152,                 /* "priority"  */
  YYSYMBOL_FLOWLABEL = 153,                /* "flowlabel"  */
  YYSYMBOL_NEXTHDR = 154,                  /* "nexthdr"  */
  YYSYMBOL_HOPLIMIT = 155,                 /* "hoplimit"  */
  YYSYMBOL_ICMP6 = 156,                    /* "icmpv6"  */
  YYSYMBOL_PPTR = 157,                     /* "param-problem"  */
  YYSYMBOL_MAXDELAY = 158,                 /* "max-delay"  */
  YYSYMBOL_TADDR = 159,                    /* "taddr"  */
  YYSYMBOL_AH = 160,                       /* "ah"  */
  YYSYMBOL_RESERVED = 161,                 /* "reserved"  */
  YYSYMBOL_SPI = 162,                      /* "spi"  */
  YYSYMBOL_ESP = 163,                      /* "esp"  */
  YYSYMBOL_COMP = 164,                     /* "comp"  */
  YYSYMBOL_FLAGS = 165,                    /* "flags"  */
  YYSYMBOL_CPI = 166,                      /* "cpi"  */
  YYSYMBOL_PORT = 167,                     /* "port"  */
  YYSYMBOL_UDP = 168,                      /* "udp"  */
  YYSYMBOL_SPORT = 169,                    /* "sport"  */
  YYSYMBOL_DPORT = 170,                    /* "dport"  */
  YYSYMBOL_UDPLITE = 171,                  /* "udplite"  */
  YYSYMBOL_CSUMCOV = 172,                  /* "csumcov"  */
  YYSYMBOL_TCP = 173,                      /* "tcp"  */
  YYSYMBOL_ACKSEQ = 174,                   /* "ackseq"  */
  YYSYMBOL_DOFF = 175,                     /* "doff"  */
  YYSYMBOL_WINDOW = 176,                   /* "window"  */
  YYSYMBOL_URGPTR = 177,                   /* "urgptr"  */
  YYSYMBOL_OPTION = 178,                   /* "option"  */
  YYSYMBOL_ECHO = 179,                     /* "echo"  */
  YYSYMBOL_EOL = 180,                      /* "eol"  */
  YYSYMBOL_MPTCP = 181,                    /* "mptcp"  */
  YYSYMBOL_NOP = 182,                      /* "nop"  */
  YYSYMBOL_SACK = 183,                     /* "sack"  */
  YYSYMBOL_SACK0 = 184,                    /* "sack0"  */
  YYSYMBOL_SACK1 = 185,                    /* "sack1"  */
  YYSYMBOL_SACK2 = 186,                    /* "sack2"  */
  YYSYMBOL_SACK3 = 187,                    /* "sack3"  */
  YYSYMBOL_SACK_PERM = 188,                /* "sack-permitted"  */
  YYSYMBOL_FASTOPEN = 189,                 /* "fastopen"  */
  YYSYMBOL_MD5SIG = 190,                   /* "md5sig"  */
  YYSYMBOL_TIMESTAMP = 191,                /* "timestamp"  */
  YYSYMBOL_COUNT = 192,                    /* "count"  */
  YYSYMBOL_LEFT = 193,                     /* "left"  */
  YYSYMBOL_RIGHT = 194,                    /* "right"  */
  YYSYMBOL_TSVAL = 195,                    /* "tsval"  */
  YYSYMBOL_TSECR = 196,                    /* "tsecr"  */
  YYSYMBOL_SUBTYPE = 197,                  /* "subtype"  */
  YYSYMBOL_DCCP = 198,                     /* "dccp"  */
  YYSYMBOL_VXLAN = 199,                    /* "vxlan"  */
  YYSYMBOL_VNI = 200,                      /* "vni"  */
  YYSYMBOL_GRE = 201,                      /* "gre"  */
  YYSYMBOL_GRETAP = 202,                   /* "gretap"  */
  YYSYMBOL_GENEVE = 203,                   /* "geneve"  */
  YYSYMBOL_SCTP = 204,                     /* "sctp"  */
  YYSYMBOL_CHUNK = 205,                    /* "chunk"  */
  YYSYMBOL_DATA = 206,                     /* "data"  */
  YYSYMBOL_INIT = 207,                     /* "init"  */
  YYSYMBOL_INIT_ACK = 208,                 /* "init-ack"  */
  YYSYMBOL_HEARTBEAT = 209,                /* "heartbeat"  */
  YYSYMBOL_HEARTBEAT_ACK = 210,            /* "heartbeat-ack"  */
  YYSYMBOL_ABORT = 211,                    /* "abort"  */
  YYSYMBOL_SHUTDOWN = 212,                 /* "shutdown"  */
  YYSYMBOL_SHUTDOWN_ACK = 213,             /* "shutdown-ack"  */
  YYSYMBOL_ERROR = 214,                    /* "error"  */
  YYSYMBOL_COOKIE_ECHO = 215,              /* "cookie-echo"  */
  YYSYMBOL_COOKIE_ACK = 216,               /* "cookie-ack"  */
  YYSYMBOL_ECNE = 217,                     /* "ecne"  */
  YYSYMBOL_CWR = 218,                      /* "cwr"  */
  YYSYMBOL_SHUTDOWN_COMPLETE = 219,        /* "shutdown-complete"  */
  YYSYMBOL_ASCONF_ACK = 220,               /* "asconf-ack"  */
  YYSYMBOL_FORWARD_TSN = 221,              /* "forward-tsn"  */
  YYSYMBOL_ASCONF = 222,                   /* "asconf"  */
  YYSYMBOL_TSN = 223,                      /* "tsn"  */
  YYSYMBOL_STREAM = 224,                   /* "stream"  */
  YYSYMBOL_SSN = 225,                      /* "ssn"  */
  YYSYMBOL_PPID = 226,                     /* "ppid"  */
  YYSYMBOL_INIT_TAG = 227,                 /* "init-tag"  */
  YYSYMBOL_A_RWND = 228,                   /* "a-rwnd"  */
  YYSYMBOL_NUM_OSTREAMS = 229,             /* "num-outbound-streams"  */
  YYSYMBOL_NUM_ISTREAMS = 230,             /* "num-inbound-streams"  */
  YYSYMBOL_INIT_TSN = 231,                 /* "initial-tsn"  */
  YYSYMBOL_CUM_TSN_ACK = 232,              /* "cum-tsn-ack"  */
  YYSYMBOL_NUM_GACK_BLOCKS = 233,          /* "num-gap-ack-blocks"  */
  YYSYMBOL_NUM_DUP_TSNS = 234,             /* "num-dup-tsns"  */
  YYSYMBOL_LOWEST_TSN = 235,               /* "lowest-tsn"  */
  YYSYMBOL_SEQNO = 236,                    /* "seqno"  */
  YYSYMBOL_NEW_CUM_TSN = 237,              /* "new-cum-tsn"  */
  YYSYMBOL_VTAG = 238,                     /* "vtag"  */
  YYSYMBOL_RT = 239,                       /* "rt"  */
  YYSYMBOL_RT0 = 240,                      /* "rt0"  */
  YYSYMBOL_RT2 = 241,                      /* "rt2"  */
  YYSYMBOL_RT4 = 242,                      /* "srh"  */
  YYSYMBOL_SEG_LEFT = 243,                 /* "seg-left"  */
  YYSYMBOL_ADDR = 244,                     /* "addr"  */
  YYSYMBOL_LAST_ENT = 245,                 /* "last-entry"  */
  YYSYMBOL_TAG = 246,                      /* "tag"  */
  YYSYMBOL_SID = 247,                      /* "sid"  */
  YYSYMBOL_HBH = 248,                      /* "hbh"  */
  YYSYMBOL_FRAG = 249,                     /* "frag"  */
  YYSYMBOL_RESERVED2 = 250,                /* "reserved2"  */
  YYSYMBOL_MORE_FRAGMENTS = 251,           /* "more-fragments"  */
  YYSYMBOL_DST = 252,                      /* "dst"  */
  YYSYMBOL_MH = 253,                       /* "mh"  */
  YYSYMBOL_META = 254,                     /* "meta"  */
  YYSYMBOL_MARK = 255,                     /* "mark"  */
  YYSYMBOL_IIF = 256,                      /* "iif"  */
  YYSYMBOL_IIFNAME = 257,                  /* "iifname"  */
  YYSYMBOL_IIFTYPE = 258,                  /* "iiftype"  */
  YYSYMBOL_OIF = 259,                      /* "oif"  */
  YYSYMBOL_OIFNAME = 260,                  /* "oifname"  */
  YYSYMBOL_OIFTYPE = 261,                  /* "oiftype"  */
  YYSYMBOL_SKUID = 262,                    /* "skuid"  */
  YYSYMBOL_SKGID = 263,                    /* "skgid"  */
  YYSYMBOL_NFTRACE = 264,                  /* "nftrace"  */
  YYSYMBOL_RTCLASSID = 265,                /* "rtclassid"  */
  YYSYMBOL_IBRIPORT = 266,                 /* "ibriport"  */
  YYSYMBOL_OBRIPORT = 267,                 /* "obriport"  */
  YYSYMBOL_IBRIDGENAME = 268,              /* "ibrname"  */
  YYSYMBOL_OBRIDGENAME = 269,              /* "obrname"  */
  YYSYMBOL_PKTTYPE = 270,                  /* "pkttype"  */
  YYSYMBOL_CPU = 271,                      /* "cpu"  */
  YYSYMBOL_IIFGROUP = 272,                 /* "iifgroup"  */
  YYSYMBOL_OIFGROUP = 273,                 /* "oifgroup"  */
  YYSYMBOL_CGROUP = 274,                   /* "cgroup"  */
  YYSYMBOL_TIME = 275,                     /* "time"  */
  YYSYMBOL_CLASSID = 276,                  /* "classid"  */
  YYSYMBOL_NEXTHOP = 277,                  /* "nexthop"  */
  YYSYMBOL_CT = 278,                       /* "ct"  */
  YYSYMBOL_L3PROTOCOL = 279,               /* "l3proto"  */
  YYSYMBOL_PROTO_SRC = 280,                /* "proto-src"  */
  YYSYMBOL_PROTO_DST = 281,                /* "proto-dst"  */
  YYSYMBOL_ZONE = 282,                     /* "zone"  */
  YYSYMBOL_DIRECTION = 283,                /* "direction"  */
  YYSYMBOL_EVENT = 284,                    /* "event"  */
  YYSYMBOL_EXPECTATION = 285,              /* "expectation"  */
  YYSYMBOL_EXPIRATION = 286,               /* "expiration"  */
  YYSYMBOL_HELPER = 287,                   /* "helper"  */
  YYSYMBOL_LABEL = 288,                    /* "label"  */
  YYSYMBOL_STATE = 289,                    /* "state"  */
  YYSYMBOL_STATUS = 290,                   /* "status"  */
  YYSYMBOL_ORIGINAL = 291,                 /* "original"  */
  YYSYMBOL_REPLY = 292,                    /* "reply"  */
  YYSYMBOL_COUNTER = 293,                  /* "counter"  */
  YYSYMBOL_NAME = 294,                     /* "name"  */
  YYSYMBOL_PACKETS = 295,                  /* "packets"  */
  YYSYMBOL_BYTES = 296,                    /* "bytes"  */
  YYSYMBOL_AVGPKT = 297,                   /* "avgpkt"  */
  YYSYMBOL_LAST = 298,                     /* "last"  */
  YYSYMBOL_NEVER = 299,                    /* "never"  */
  YYSYMBOL_COUNTERS = 300,                 /* "counters"  */
  YYSYMBOL_QUOTAS = 301,                   /* "quotas"  */
  YYSYMBOL_LIMITS = 302,                   /* "limits"  */
  YYSYMBOL_SYNPROXYS = 303,                /* "synproxys"  */
  YYSYMBOL_HELPERS = 304,                  /* "helpers"  */
  YYSYMBOL_LOG = 305,                      /* "log"  */
  YYSYMBOL_PREFIX = 306,                   /* "prefix"  */
  YYSYMBOL_GROUP = 307,                    /* "group"  */
  YYSYMBOL_SNAPLEN = 308,                  /* "snaplen"  */
  YYSYMBOL_QUEUE_THRESHOLD = 309,          /* "queue-threshold"  */
  YYSYMBOL_LEVEL = 310,                    /* "level"  */
  YYSYMBOL_LIMIT = 311,                    /* "limit"  */
  YYSYMBOL_RATE = 312,                     /* "rate"  */
  YYSYMBOL_BURST = 313,                    /* "burst"  */
  YYSYMBOL_OVER = 314,                     /* "over"  */
  YYSYMBOL_UNTIL = 315,                    /* "until"  */
  YYSYMBOL_QUOTA = 316,                    /* "quota"  */
  YYSYMBOL_USED = 317,                     /* "used"  */
  YYSYMBOL_SECMARK = 318,                  /* "secmark"  */
  YYSYMBOL_SECMARKS = 319,                 /* "secmarks"  */
  YYSYMBOL_SECOND = 320,                   /* "second"  */
  YYSYMBOL_MINUTE = 321,                   /* "minute"  */
  YYSYMBOL_HOUR = 322,                     /* "hour"  */
  YYSYMBOL_DAY = 323,                      /* "day"  */
  YYSYMBOL_WEEK = 324,                     /* "week"  */
  YYSYMBOL__REJECT = 325,                  /* "reject"  */
  YYSYMBOL_WITH = 326,                     /* "with"  */
  YYSYMBOL_ICMPX = 327,                    /* "icmpx"  */
  YYSYMBOL_SNAT = 328,                     /* "snat"  */
  YYSYMBOL_DNAT = 329,                     /* "dnat"  */
  YYSYMBOL_MASQUERADE = 330,               /* "masquerade"  */
  YYSYMBOL_REDIRECT = 331,                 /* "redirect"  */
  YYSYMBOL_RANDOM = 332,                   /* "random"  */
  YYSYMBOL_FULLY_RANDOM = 333,             /* "fully-random"  */
  YYSYMBOL_PERSISTENT = 334,               /* "persistent"  */
  YYSYMBOL_QUEUE = 335,                    /* "queue"  */
  YYSYMBOL_QUEUENUM = 336,                 /* "num"  */
  YYSYMBOL_BYPASS = 337,                   /* "bypass"  */
  YYSYMBOL_FANOUT = 338,                   /* "fanout"  */
  YYSYMBOL_DUP = 339,                      /* "dup"  */
  YYSYMBOL_FWD = 340,                      /* "fwd"  */
  YYSYMBOL_NUMGEN = 341,                   /* "numgen"  */
  YYSYMBOL_INC = 342,                      /* "inc"  */
  YYSYMBOL_MOD = 343,                      /* "mod"  */
  YYSYMBOL_OFFSET = 344,                   /* "offset"  */
  YYSYMBOL_JHASH = 345,                    /* "jhash"  */
  YYSYMBOL_SYMHASH = 346,                  /* "symhash"  */
  YYSYMBOL_SEED = 347,                     /* "seed"  */
  YYSYMBOL_POSITION = 348,                 /* "position"  */
  YYSYMBOL_INDEX = 349,                    /* "index"  */
  YYSYMBOL_COMMENT = 350,                  /* "comment"  */
  YYSYMBOL_XML = 351,                      /* "xml"  */
  YYSYMBOL_JSON = 352,                     /* "json"  */
  YYSYMBOL_VM = 353,                       /* "vm"  */
  YYSYMBOL_NOTRACK = 354,                  /* "notrack"  */
  YYSYMBOL_EXISTS = 355,                   /* "exists"  */
  YYSYMBOL_MISSING = 356,                  /* "missing"  */
  YYSYMBOL_EXTHDR = 357,                   /* "exthdr"  */
  YYSYMBOL_IPSEC = 358,                    /* "ipsec"  */
  YYSYMBOL_REQID = 359,                    /* "reqid"  */
  YYSYMBOL_SPNUM = 360,                    /* "spnum"  */
  YYSYMBOL_IN = 361,                       /* "in"  */
  YYSYMBOL_OUT = 362,                      /* "out"  */
  YYSYMBOL_XT = 363,                       /* "xt"  */
  YYSYMBOL_364_ = 364,                     /* '='  */
  YYSYMBOL_365_ = 365,                     /* '{'  */
  YYSYMBOL_366_ = 366,                     /* '}'  */
  YYSYMBOL_367_ = 367,                     /* '('  */
  YYSYMBOL_368_ = 368,                     /* ')'  */
  YYSYMBOL_369_ = 369,                     /* '|'  */
  YYSYMBOL_370_ = 370,                     /* '$'  */
  YYSYMBOL_371_ = 371,                     /* '['  */
  YYSYMBOL_372_ = 372,                     /* ']'  */
  YYSYMBOL_YYACCEPT = 373,                 /* $accept  */
  YYSYMBOL_input = 374,                    /* input  */
  YYSYMBOL_stmt_separator = 375,           /* stmt_separator  */
  YYSYMBOL_opt_newline = 376,              /* opt_newline  */
  YYSYMBOL_close_scope_ah = 377,           /* close_scope_ah  */
  YYSYMBOL_close_scope_arp = 378,          /* close_scope_arp  */
  YYSYMBOL_close_scope_at = 379,           /* close_scope_at  */
  YYSYMBOL_close_scope_comp = 380,         /* close_scope_comp  */
  YYSYMBOL_close_scope_ct = 381,           /* close_scope_ct  */
  YYSYMBOL_close_scope_counter = 382,      /* close_scope_counter  */
  YYSYMBOL_close_scope_last = 383,         /* close_scope_last  */
  YYSYMBOL_close_scope_dccp = 384,         /* close_scope_dccp  */
  YYSYMBOL_close_scope_destroy = 385,      /* close_scope_destroy  */
  YYSYMBOL_close_scope_dst = 386,          /* close_scope_dst  */
  YYSYMBOL_close_scope_dup = 387,          /* close_scope_dup  */
  YYSYMBOL_close_scope_esp = 388,          /* close_scope_esp  */
  YYSYMBOL_close_scope_eth = 389,          /* close_scope_eth  */
  YYSYMBOL_close_scope_export = 390,       /* close_scope_export  */
  YYSYMBOL_close_scope_fib = 391,          /* close_scope_fib  */
  YYSYMBOL_close_scope_frag = 392,         /* close_scope_frag  */
  YYSYMBOL_close_scope_fwd = 393,          /* close_scope_fwd  */
  YYSYMBOL_close_scope_gre = 394,          /* close_scope_gre  */
  YYSYMBOL_close_scope_hash = 395,         /* close_scope_hash  */
  YYSYMBOL_close_scope_hbh = 396,          /* close_scope_hbh  */
  YYSYMBOL_close_scope_ip = 397,           /* close_scope_ip  */
  YYSYMBOL_close_scope_ip6 = 398,          /* close_scope_ip6  */
  YYSYMBOL_close_scope_vlan = 399,         /* close_scope_vlan  */
  YYSYMBOL_close_scope_icmp = 400,         /* close_scope_icmp  */
  YYSYMBOL_close_scope_igmp = 401,         /* close_scope_igmp  */
  YYSYMBOL_close_scope_import = 402,       /* close_scope_import  */
  YYSYMBOL_close_scope_ipsec = 403,        /* close_scope_ipsec  */
  YYSYMBOL_close_scope_list = 404,         /* close_scope_list  */
  YYSYMBOL_close_scope_limit = 405,        /* close_scope_limit  */
  YYSYMBOL_close_scope_meta = 406,         /* close_scope_meta  */
  YYSYMBOL_close_scope_mh = 407,           /* close_scope_mh  */
  YYSYMBOL_close_scope_monitor = 408,      /* close_scope_monitor  */
  YYSYMBOL_close_scope_nat = 409,          /* close_scope_nat  */
  YYSYMBOL_close_scope_numgen = 410,       /* close_scope_numgen  */
  YYSYMBOL_close_scope_osf = 411,          /* close_scope_osf  */
  YYSYMBOL_close_scope_policy = 412,       /* close_scope_policy  */
  YYSYMBOL_close_scope_quota = 413,        /* close_scope_quota  */
  YYSYMBOL_close_scope_queue = 414,        /* close_scope_queue  */
  YYSYMBOL_close_scope_reject = 415,       /* close_scope_reject  */
  YYSYMBOL_close_scope_reset = 416,        /* close_scope_reset  */
  YYSYMBOL_close_scope_rt = 417,           /* close_scope_rt  */
  YYSYMBOL_close_scope_sctp = 418,         /* close_scope_sctp  */
  YYSYMBOL_close_scope_sctp_chunk = 419,   /* close_scope_sctp_chunk  */
  YYSYMBOL_close_scope_secmark = 420,      /* close_scope_secmark  */
  YYSYMBOL_close_scope_socket = 421,       /* close_scope_socket  */
  YYSYMBOL_close_scope_tcp = 422,          /* close_scope_tcp  */
  YYSYMBOL_close_scope_tproxy = 423,       /* close_scope_tproxy  */
  YYSYMBOL_close_scope_type = 424,         /* close_scope_type  */
  YYSYMBOL_close_scope_th = 425,           /* close_scope_th  */
  YYSYMBOL_close_scope_udp = 426,          /* close_scope_udp  */
  YYSYMBOL_close_scope_udplite = 427,      /* close_scope_udplite  */
  YYSYMBOL_close_scope_log = 428,          /* close_scope_log  */
  YYSYMBOL_close_scope_synproxy = 429,     /* close_scope_synproxy  */
  YYSYMBOL_close_scope_xt = 430,           /* close_scope_xt  */
  YYSYMBOL_common_block = 431,             /* common_block  */
  YYSYMBOL_line = 432,                     /* line  */
  YYSYMBOL_base_cmd = 433,                 /* base_cmd  */
  YYSYMBOL_add_cmd = 434,                  /* add_cmd  */
  YYSYMBOL_replace_cmd = 435,              /* replace_cmd  */
  YYSYMBOL_create_cmd = 436,               /* create_cmd  */
  YYSYMBOL_insert_cmd = 437,               /* insert_cmd  */
  YYSYMBOL_table_or_id_spec = 438,         /* table_or_id_spec  */
  YYSYMBOL_chain_or_id_spec = 439,         /* chain_or_id_spec  */
  YYSYMBOL_set_or_id_spec = 440,           /* set_or_id_spec  */
  YYSYMBOL_obj_or_id_spec = 441,           /* obj_or_id_spec  */
  YYSYMBOL_delete_cmd = 442,               /* delete_cmd  */
  YYSYMBOL_destroy_cmd = 443,              /* destroy_cmd  */
  YYSYMBOL_get_cmd = 444,                  /* get_cmd  */
  YYSYMBOL_list_cmd_spec_table = 445,      /* list_cmd_spec_table  */
  YYSYMBOL_list_cmd_spec_any = 446,        /* list_cmd_spec_any  */
  YYSYMBOL_list_cmd = 447,                 /* list_cmd  */
  YYSYMBOL_basehook_device_name = 448,     /* basehook_device_name  */
  YYSYMBOL_basehook_spec = 449,            /* basehook_spec  */
  YYSYMBOL_reset_cmd = 450,                /* reset_cmd  */
  YYSYMBOL_flush_cmd = 451,                /* flush_cmd  */
  YYSYMBOL_rename_cmd = 452,               /* rename_cmd  */
  YYSYMBOL_import_cmd = 453,               /* import_cmd  */
  YYSYMBOL_export_cmd = 454,               /* export_cmd  */
  YYSYMBOL_monitor_cmd = 455,              /* monitor_cmd  */
  YYSYMBOL_monitor_event = 456,            /* monitor_event  */
  YYSYMBOL_monitor_object = 457,           /* monitor_object  */
  YYSYMBOL_monitor_format = 458,           /* monitor_format  */
  YYSYMBOL_markup_format = 459,            /* markup_format  */
  YYSYMBOL_describe_cmd = 460,             /* describe_cmd  */
  YYSYMBOL_table_block_alloc = 461,        /* table_block_alloc  */
  YYSYMBOL_table_options = 462,            /* table_options  */
  YYSYMBOL_table_flags = 463,              /* table_flags  */
  YYSYMBOL_table_flag = 464,               /* table_flag  */
  YYSYMBOL_table_block = 465,              /* table_block  */
  YYSYMBOL_chain_block_alloc = 466,        /* chain_block_alloc  */
  YYSYMBOL_chain_block = 467,              /* chain_block  */
  YYSYMBOL_subchain_block = 468,           /* subchain_block  */
  YYSYMBOL_typeof_verdict_expr = 469,      /* typeof_verdict_expr  */
  YYSYMBOL_typeof_data_expr = 470,         /* typeof_data_expr  */
  YYSYMBOL_primary_typeof_expr = 471,      /* primary_typeof_expr  */
  YYSYMBOL_typeof_expr = 472,              /* typeof_expr  */
  YYSYMBOL_set_block_alloc = 473,          /* set_block_alloc  */
  YYSYMBOL_typeof_key_expr = 474,          /* typeof_key_expr  */
  YYSYMBOL_set_block = 475,                /* set_block  */
  YYSYMBOL_set_block_expr = 476,           /* set_block_expr  */
  YYSYMBOL_set_flag_list = 477,            /* set_flag_list  */
  YYSYMBOL_set_flag = 478,                 /* set_flag  */
  YYSYMBOL_map_block_alloc = 479,          /* map_block_alloc  */
  YYSYMBOL_ct_obj_type_map = 480,          /* ct_obj_type_map  */
  YYSYMBOL_map_block_obj_type = 481,       /* map_block_obj_type  */
  YYSYMBOL_map_block_obj_typeof = 482,     /* map_block_obj_typeof  */
  YYSYMBOL_map_block_data_interval = 483,  /* map_block_data_interval  */
  YYSYMBOL_map_block = 484,                /* map_block  */
  YYSYMBOL_set_mechanism = 485,            /* set_mechanism  */
  YYSYMBOL_set_policy_spec = 486,          /* set_policy_spec  */
  YYSYMBOL_flowtable_block_alloc = 487,    /* flowtable_block_alloc  */
  YYSYMBOL_flowtable_block = 488,          /* flowtable_block  */
  YYSYMBOL_flowtable_expr = 489,           /* flowtable_expr  */
  YYSYMBOL_flowtable_list_expr = 490,      /* flowtable_list_expr  */
  YYSYMBOL_flowtable_expr_member = 491,    /* flowtable_expr_member  */
  YYSYMBOL_data_type_atom_expr = 492,      /* data_type_atom_expr  */
  YYSYMBOL_data_type_expr = 493,           /* data_type_expr  */
  YYSYMBOL_obj_block_alloc = 494,          /* obj_block_alloc  */
  YYSYMBOL_counter_block = 495,            /* counter_block  */
  YYSYMBOL_quota_block = 496,              /* quota_block  */
  YYSYMBOL_ct_helper_block = 497,          /* ct_helper_block  */
  YYSYMBOL_ct_timeout_block = 498,         /* ct_timeout_block  */
  YYSYMBOL_ct_expect_block = 499,          /* ct_expect_block  */
  YYSYMBOL_limit_block = 500,              /* limit_block  */
  YYSYMBOL_secmark_block = 501,            /* secmark_block  */
  YYSYMBOL_synproxy_block = 502,           /* synproxy_block  */
  YYSYMBOL_type_identifier = 503,          /* type_identifier  */
  YYSYMBOL_hook_spec = 504,                /* hook_spec  */
  YYSYMBOL_prio_spec = 505,                /* prio_spec  */
  YYSYMBOL_extended_prio_name = 506,       /* extended_prio_name  */
  YYSYMBOL_extended_prio_spec = 507,       /* extended_prio_spec  */
  YYSYMBOL_int_num = 508,                  /* int_num  */
  YYSYMBOL_dev_spec = 509,                 /* dev_spec  */
  YYSYMBOL_flags_spec = 510,               /* flags_spec  */
  YYSYMBOL_policy_spec = 511,              /* policy_spec  */
  YYSYMBOL_policy_expr = 512,              /* policy_expr  */
  YYSYMBOL_chain_policy = 513,             /* chain_policy  */
  YYSYMBOL_identifier = 514,               /* identifier  */
  YYSYMBOL_string = 515,                   /* string  */
  YYSYMBOL_time_spec = 516,                /* time_spec  */
  YYSYMBOL_time_spec_or_num_s = 517,       /* time_spec_or_num_s  */
  YYSYMBOL_family_spec = 518,              /* family_spec  */
  YYSYMBOL_family_spec_explicit = 519,     /* family_spec_explicit  */
  YYSYMBOL_table_spec = 520,               /* table_spec  */
  YYSYMBOL_tableid_spec = 521,             /* tableid_spec  */
  YYSYMBOL_chain_spec = 522,               /* chain_spec  */
  YYSYMBOL_chainid_spec = 523,             /* chainid_spec  */
  YYSYMBOL_chain_identifier = 524,         /* chain_identifier  */
  YYSYMBOL_set_spec = 525,                 /* set_spec  */
  YYSYMBOL_setid_spec = 526,               /* setid_spec  */
  YYSYMBOL_set_identifier = 527,           /* set_identifier  */
  YYSYMBOL_flowtable_spec = 528,           /* flowtable_spec  */
  YYSYMBOL_flowtableid_spec = 529,         /* flowtableid_spec  */
  YYSYMBOL_flowtable_identifier = 530,     /* flowtable_identifier  */
  YYSYMBOL_obj_spec = 531,                 /* obj_spec  */
  YYSYMBOL_objid_spec = 532,               /* objid_spec  */
  YYSYMBOL_obj_identifier = 533,           /* obj_identifier  */
  YYSYMBOL_handle_spec = 534,              /* handle_spec  */
  YYSYMBOL_position_spec = 535,            /* position_spec  */
  YYSYMBOL_index_spec = 536,               /* index_spec  */
  YYSYMBOL_rule_position = 537,            /* rule_position  */
  YYSYMBOL_ruleid_spec = 538,              /* ruleid_spec  */
  YYSYMBOL_comment_spec = 539,             /* comment_spec  */
  YYSYMBOL_ruleset_spec = 540,             /* ruleset_spec  */
  YYSYMBOL_rule = 541,                     /* rule  */
  YYSYMBOL_rule_alloc = 542,               /* rule_alloc  */
  YYSYMBOL_stmt_list = 543,                /* stmt_list  */
  YYSYMBOL_stateful_stmt_list = 544,       /* stateful_stmt_list  */
  YYSYMBOL_objref_stmt_counter = 545,      /* objref_stmt_counter  */
  YYSYMBOL_objref_stmt_limit = 546,        /* objref_stmt_limit  */
  YYSYMBOL_objref_stmt_quota = 547,        /* objref_stmt_quota  */
  YYSYMBOL_objref_stmt_synproxy = 548,     /* objref_stmt_synproxy  */
  YYSYMBOL_objref_stmt_ct = 549,           /* objref_stmt_ct  */
  YYSYMBOL_objref_stmt = 550,              /* objref_stmt  */
  YYSYMBOL_stateful_stmt = 551,            /* stateful_stmt  */
  YYSYMBOL_stmt = 552,                     /* stmt  */
  YYSYMBOL_xt_stmt = 553,                  /* xt_stmt  */
  YYSYMBOL_chain_stmt_type = 554,          /* chain_stmt_type  */
  YYSYMBOL_chain_stmt = 555,               /* chain_stmt  */
  YYSYMBOL_verdict_stmt = 556,             /* verdict_stmt  */
  YYSYMBOL_verdict_map_stmt = 557,         /* verdict_map_stmt  */
  YYSYMBOL_verdict_map_expr = 558,         /* verdict_map_expr  */
  YYSYMBOL_verdict_map_list_expr = 559,    /* verdict_map_list_expr  */
  YYSYMBOL_verdict_map_list_member_expr = 560, /* verdict_map_list_member_expr  */
  YYSYMBOL_ct_limit_stmt_alloc = 561,      /* ct_limit_stmt_alloc  */
  YYSYMBOL_connlimit_stmt = 562,           /* connlimit_stmt  */
  YYSYMBOL_ct_limit_args = 563,            /* ct_limit_args  */
  YYSYMBOL_counter_stmt = 564,             /* counter_stmt  */
  YYSYMBOL_counter_stmt_alloc = 565,       /* counter_stmt_alloc  */
  YYSYMBOL_counter_args = 566,             /* counter_args  */
  YYSYMBOL_counter_arg = 567,              /* counter_arg  */
  YYSYMBOL_last_stmt_alloc = 568,          /* last_stmt_alloc  */
  YYSYMBOL_last_stmt = 569,                /* last_stmt  */
  YYSYMBOL_last_args = 570,                /* last_args  */
  YYSYMBOL_log_stmt = 571,                 /* log_stmt  */
  YYSYMBOL_log_stmt_alloc = 572,           /* log_stmt_alloc  */
  YYSYMBOL_log_args = 573,                 /* log_args  */
  YYSYMBOL_log_arg = 574,                  /* log_arg  */
  YYSYMBOL_level_type = 575,               /* level_type  */
  YYSYMBOL_log_flags = 576,                /* log_flags  */
  YYSYMBOL_log_flags_tcp = 577,            /* log_flags_tcp  */
  YYSYMBOL_log_flag_tcp = 578,             /* log_flag_tcp  */
  YYSYMBOL_limit_stmt_alloc = 579,         /* limit_stmt_alloc  */
  YYSYMBOL_limit_stmt = 580,               /* limit_stmt  */
  YYSYMBOL_limit_args = 581,               /* limit_args  */
  YYSYMBOL_quota_mode = 582,               /* quota_mode  */
  YYSYMBOL_quota_unit = 583,               /* quota_unit  */
  YYSYMBOL_quota_used = 584,               /* quota_used  */
  YYSYMBOL_quota_stmt_alloc = 585,         /* quota_stmt_alloc  */
  YYSYMBOL_quota_stmt = 586,               /* quota_stmt  */
  YYSYMBOL_quota_args = 587,               /* quota_args  */
  YYSYMBOL_limit_mode = 588,               /* limit_mode  */
  YYSYMBOL_limit_burst_pkts = 589,         /* limit_burst_pkts  */
  YYSYMBOL_limit_rate_pkts = 590,          /* limit_rate_pkts  */
  YYSYMBOL_limit_burst_bytes = 591,        /* limit_burst_bytes  */
  YYSYMBOL_limit_rate_bytes = 592,         /* limit_rate_bytes  */
  YYSYMBOL_limit_bytes = 593,              /* limit_bytes  */
  YYSYMBOL_time_unit = 594,                /* time_unit  */
  YYSYMBOL_reject_stmt = 595,              /* reject_stmt  */
  YYSYMBOL_reject_stmt_alloc = 596,        /* reject_stmt_alloc  */
  YYSYMBOL_reject_with_expr = 597,         /* reject_with_expr  */
  YYSYMBOL_reject_opts = 598,              /* reject_opts  */
  YYSYMBOL_nat_stmt = 599,                 /* nat_stmt  */
  YYSYMBOL_nat_stmt_alloc = 600,           /* nat_stmt_alloc  */
  YYSYMBOL_tproxy_stmt = 601,              /* tproxy_stmt  */
  YYSYMBOL_synproxy_stmt = 602,            /* synproxy_stmt  */
  YYSYMBOL_synproxy_stmt_alloc = 603,      /* synproxy_stmt_alloc  */
  YYSYMBOL_synproxy_args = 604,            /* synproxy_args  */
  YYSYMBOL_synproxy_arg = 605,             /* synproxy_arg  */
  YYSYMBOL_synproxy_config = 606,          /* synproxy_config  */
  YYSYMBOL_synproxy_obj = 607,             /* synproxy_obj  */
  YYSYMBOL_synproxy_ts = 608,              /* synproxy_ts  */
  YYSYMBOL_synproxy_sack = 609,            /* synproxy_sack  */
  YYSYMBOL_primary_stmt_expr = 610,        /* primary_stmt_expr  */
  YYSYMBOL_shift_stmt_expr = 611,          /* shift_stmt_expr  */
  YYSYMBOL_and_stmt_expr = 612,            /* and_stmt_expr  */
  YYSYMBOL_exclusive_or_stmt_expr = 613,   /* exclusive_or_stmt_expr  */
  YYSYMBOL_inclusive_or_stmt_expr = 614,   /* inclusive_or_stmt_expr  */
  YYSYMBOL_basic_stmt_expr = 615,          /* basic_stmt_expr  */
  YYSYMBOL_concat_stmt_expr = 616,         /* concat_stmt_expr  */
  YYSYMBOL_map_stmt_expr_set = 617,        /* map_stmt_expr_set  */
  YYSYMBOL_map_stmt_expr = 618,            /* map_stmt_expr  */
  YYSYMBOL_prefix_stmt_expr = 619,         /* prefix_stmt_expr  */
  YYSYMBOL_range_stmt_expr = 620,          /* range_stmt_expr  */
  YYSYMBOL_multiton_stmt_expr = 621,       /* multiton_stmt_expr  */
  YYSYMBOL_stmt_expr = 622,                /* stmt_expr  */
  YYSYMBOL_nat_stmt_args = 623,            /* nat_stmt_args  */
  YYSYMBOL_masq_stmt = 624,                /* masq_stmt  */
  YYSYMBOL_masq_stmt_alloc = 625,          /* masq_stmt_alloc  */
  YYSYMBOL_masq_stmt_args = 626,           /* masq_stmt_args  */
  YYSYMBOL_redir_stmt = 627,               /* redir_stmt  */
  YYSYMBOL_redir_stmt_alloc = 628,         /* redir_stmt_alloc  */
  YYSYMBOL_redir_stmt_arg = 629,           /* redir_stmt_arg  */
  YYSYMBOL_dup_stmt = 630,                 /* dup_stmt  */
  YYSYMBOL_fwd_stmt = 631,                 /* fwd_stmt  */
  YYSYMBOL_nf_nat_flags = 632,             /* nf_nat_flags  */
  YYSYMBOL_nf_nat_flag = 633,              /* nf_nat_flag  */
  YYSYMBOL_queue_stmt = 634,               /* queue_stmt  */
  YYSYMBOL_queue_stmt_compat = 635,        /* queue_stmt_compat  */
  YYSYMBOL_queue_stmt_alloc = 636,         /* queue_stmt_alloc  */
  YYSYMBOL_queue_stmt_args = 637,          /* queue_stmt_args  */
  YYSYMBOL_queue_stmt_arg = 638,           /* queue_stmt_arg  */
  YYSYMBOL_queue_expr = 639,               /* queue_expr  */
  YYSYMBOL_queue_stmt_expr_simple = 640,   /* queue_stmt_expr_simple  */
  YYSYMBOL_queue_stmt_expr = 641,          /* queue_stmt_expr  */
  YYSYMBOL_queue_stmt_flags = 642,         /* queue_stmt_flags  */
  YYSYMBOL_queue_stmt_flag = 643,          /* queue_stmt_flag  */
  YYSYMBOL_set_elem_expr_stmt = 644,       /* set_elem_expr_stmt  */
  YYSYMBOL_set_elem_expr_stmt_alloc = 645, /* set_elem_expr_stmt_alloc  */
  YYSYMBOL_set_stmt = 646,                 /* set_stmt  */
  YYSYMBOL_set_stmt_op = 647,              /* set_stmt_op  */
  YYSYMBOL_map_stmt = 648,                 /* map_stmt  */
  YYSYMBOL_meter_stmt = 649,               /* meter_stmt  */
  YYSYMBOL_match_stmt = 650,               /* match_stmt  */
  YYSYMBOL_variable_expr = 651,            /* variable_expr  */
  YYSYMBOL_symbol_expr = 652,              /* symbol_expr  */
  YYSYMBOL_set_ref_expr = 653,             /* set_ref_expr  */
  YYSYMBOL_set_ref_symbol_expr = 654,      /* set_ref_symbol_expr  */
  YYSYMBOL_integer_expr = 655,             /* integer_expr  */
  YYSYMBOL_selector_expr = 656,            /* selector_expr  */
  YYSYMBOL_primary_expr = 657,             /* primary_expr  */
  YYSYMBOL_fib_expr = 658,                 /* fib_expr  */
  YYSYMBOL_fib_result = 659,               /* fib_result  */
  YYSYMBOL_fib_flag = 660,                 /* fib_flag  */
  YYSYMBOL_fib_tuple = 661,                /* fib_tuple  */
  YYSYMBOL_osf_expr = 662,                 /* osf_expr  */
  YYSYMBOL_osf_ttl = 663,                  /* osf_ttl  */
  YYSYMBOL_shift_expr = 664,               /* shift_expr  */
  YYSYMBOL_and_expr = 665,                 /* and_expr  */
  YYSYMBOL_exclusive_or_expr = 666,        /* exclusive_or_expr  */
  YYSYMBOL_inclusive_or_expr = 667,        /* inclusive_or_expr  */
  YYSYMBOL_basic_expr = 668,               /* basic_expr  */
  YYSYMBOL_concat_expr = 669,              /* concat_expr  */
  YYSYMBOL_prefix_rhs_expr = 670,          /* prefix_rhs_expr  */
  YYSYMBOL_range_rhs_expr = 671,           /* range_rhs_expr  */
  YYSYMBOL_multiton_rhs_expr = 672,        /* multiton_rhs_expr  */
  YYSYMBOL_map_expr = 673,                 /* map_expr  */
  YYSYMBOL_expr = 674,                     /* expr  */
  YYSYMBOL_set_expr = 675,                 /* set_expr  */
  YYSYMBOL_set_list_expr = 676,            /* set_list_expr  */
  YYSYMBOL_set_list_member_expr = 677,     /* set_list_member_expr  */
  YYSYMBOL_meter_key_expr = 678,           /* meter_key_expr  */
  YYSYMBOL_meter_key_expr_alloc = 679,     /* meter_key_expr_alloc  */
  YYSYMBOL_set_elem_expr = 680,            /* set_elem_expr  */
  YYSYMBOL_set_elem_key_expr = 681,        /* set_elem_key_expr  */
  YYSYMBOL_set_elem_expr_alloc = 682,      /* set_elem_expr_alloc  */
  YYSYMBOL_set_elem_options = 683,         /* set_elem_options  */
  YYSYMBOL_set_elem_time_spec = 684,       /* set_elem_time_spec  */
  YYSYMBOL_set_elem_option = 685,          /* set_elem_option  */
  YYSYMBOL_set_elem_expr_options = 686,    /* set_elem_expr_options  */
  YYSYMBOL_set_elem_stmt_list = 687,       /* set_elem_stmt_list  */
  YYSYMBOL_set_elem_stmt = 688,            /* set_elem_stmt  */
  YYSYMBOL_set_elem_expr_option = 689,     /* set_elem_expr_option  */
  YYSYMBOL_set_lhs_expr = 690,             /* set_lhs_expr  */
  YYSYMBOL_set_rhs_expr = 691,             /* set_rhs_expr  */
  YYSYMBOL_initializer_expr = 692,         /* initializer_expr  */
  YYSYMBOL_counter_config = 693,           /* counter_config  */
  YYSYMBOL_counter_obj = 694,              /* counter_obj  */
  YYSYMBOL_quota_config = 695,             /* quota_config  */
  YYSYMBOL_quota_obj = 696,                /* quota_obj  */
  YYSYMBOL_secmark_config = 697,           /* secmark_config  */
  YYSYMBOL_secmark_obj = 698,              /* secmark_obj  */
  YYSYMBOL_ct_obj_type = 699,              /* ct_obj_type  */
  YYSYMBOL_ct_cmd_type = 700,              /* ct_cmd_type  */
  YYSYMBOL_ct_l4protoname = 701,           /* ct_l4protoname  */
  YYSYMBOL_ct_helper_config = 702,         /* ct_helper_config  */
  YYSYMBOL_timeout_states = 703,           /* timeout_states  */
  YYSYMBOL_timeout_state = 704,            /* timeout_state  */
  YYSYMBOL_ct_timeout_config = 705,        /* ct_timeout_config  */
  YYSYMBOL_ct_expect_config = 706,         /* ct_expect_config  */
  YYSYMBOL_ct_obj_alloc = 707,             /* ct_obj_alloc  */
  YYSYMBOL_limit_config = 708,             /* limit_config  */
  YYSYMBOL_limit_obj = 709,                /* limit_obj  */
  YYSYMBOL_relational_expr = 710,          /* relational_expr  */
  YYSYMBOL_list_rhs_expr = 711,            /* list_rhs_expr  */
  YYSYMBOL_rhs_expr = 712,                 /* rhs_expr  */
  YYSYMBOL_shift_rhs_expr = 713,           /* shift_rhs_expr  */
  YYSYMBOL_and_rhs_expr = 714,             /* and_rhs_expr  */
  YYSYMBOL_exclusive_or_rhs_expr = 715,    /* exclusive_or_rhs_expr  */
  YYSYMBOL_inclusive_or_rhs_expr = 716,    /* inclusive_or_rhs_expr  */
  YYSYMBOL_basic_rhs_expr = 717,           /* basic_rhs_expr  */
  YYSYMBOL_concat_rhs_expr = 718,          /* concat_rhs_expr  */
  YYSYMBOL_boolean_keys = 719,             /* boolean_keys  */
  YYSYMBOL_boolean_expr = 720,             /* boolean_expr  */
  YYSYMBOL_keyword_expr = 721,             /* keyword_expr  */
  YYSYMBOL_primary_rhs_expr = 722,         /* primary_rhs_expr  */
  YYSYMBOL_relational_op = 723,            /* relational_op  */
  YYSYMBOL_verdict_expr = 724,             /* verdict_expr  */
  YYSYMBOL_chain_expr = 725,               /* chain_expr  */
  YYSYMBOL_meta_expr = 726,                /* meta_expr  */
  YYSYMBOL_meta_key = 727,                 /* meta_key  */
  YYSYMBOL_meta_key_qualified = 728,       /* meta_key_qualified  */
  YYSYMBOL_meta_key_unqualified = 729,     /* meta_key_unqualified  */
  YYSYMBOL_meta_stmt = 730,                /* meta_stmt  */
  YYSYMBOL_socket_expr = 731,              /* socket_expr  */
  YYSYMBOL_socket_key = 732,               /* socket_key  */
  YYSYMBOL_offset_opt = 733,               /* offset_opt  */
  YYSYMBOL_numgen_type = 734,              /* numgen_type  */
  YYSYMBOL_numgen_expr = 735,              /* numgen_expr  */
  YYSYMBOL_xfrm_spnum = 736,               /* xfrm_spnum  */
  YYSYMBOL_xfrm_dir = 737,                 /* xfrm_dir  */
  YYSYMBOL_xfrm_state_key = 738,           /* xfrm_state_key  */
  YYSYMBOL_xfrm_state_proto_key = 739,     /* xfrm_state_proto_key  */
  YYSYMBOL_xfrm_expr = 740,                /* xfrm_expr  */
  YYSYMBOL_hash_expr = 741,                /* hash_expr  */
  YYSYMBOL_nf_key_proto = 742,             /* nf_key_proto  */
  YYSYMBOL_rt_expr = 743,                  /* rt_expr  */
  YYSYMBOL_rt_key = 744,                   /* rt_key  */
  YYSYMBOL_ct_expr = 745,                  /* ct_expr  */
  YYSYMBOL_ct_dir = 746,                   /* ct_dir  */
  YYSYMBOL_ct_key = 747,                   /* ct_key  */
  YYSYMBOL_ct_key_dir = 748,               /* ct_key_dir  */
  YYSYMBOL_ct_key_proto_field = 749,       /* ct_key_proto_field  */
  YYSYMBOL_ct_key_dir_optional = 750,      /* ct_key_dir_optional  */
  YYSYMBOL_symbol_stmt_expr = 751,         /* symbol_stmt_expr  */
  YYSYMBOL_list_stmt_expr = 752,           /* list_stmt_expr  */
  YYSYMBOL_ct_stmt = 753,                  /* ct_stmt  */
  YYSYMBOL_payload_stmt = 754,             /* payload_stmt  */
  YYSYMBOL_payload_expr = 755,             /* payload_expr  */
  YYSYMBOL_payload_raw_len = 756,          /* payload_raw_len  */
  YYSYMBOL_payload_raw_expr = 757,         /* payload_raw_expr  */
  YYSYMBOL_payload_base_spec = 758,        /* payload_base_spec  */
  YYSYMBOL_eth_hdr_expr = 759,             /* eth_hdr_expr  */
  YYSYMBOL_eth_hdr_field = 760,            /* eth_hdr_field  */
  YYSYMBOL_vlan_hdr_expr = 761,            /* vlan_hdr_expr  */
  YYSYMBOL_vlan_hdr_field = 762,           /* vlan_hdr_field  */
  YYSYMBOL_arp_hdr_expr = 763,             /* arp_hdr_expr  */
  YYSYMBOL_arp_hdr_field = 764,            /* arp_hdr_field  */
  YYSYMBOL_ip_hdr_expr = 765,              /* ip_hdr_expr  */
  YYSYMBOL_ip_hdr_field = 766,             /* ip_hdr_field  */
  YYSYMBOL_ip_option_type = 767,           /* ip_option_type  */
  YYSYMBOL_ip_option_field = 768,          /* ip_option_field  */
  YYSYMBOL_icmp_hdr_expr = 769,            /* icmp_hdr_expr  */
  YYSYMBOL_icmp_hdr_field = 770,           /* icmp_hdr_field  */
  YYSYMBOL_igmp_hdr_expr = 771,            /* igmp_hdr_expr  */
  YYSYMBOL_igmp_hdr_field = 772,           /* igmp_hdr_field  */
  YYSYMBOL_ip6_hdr_expr = 773,             /* ip6_hdr_expr  */
  YYSYMBOL_ip6_hdr_field = 774,            /* ip6_hdr_field  */
  YYSYMBOL_icmp6_hdr_expr = 775,           /* icmp6_hdr_expr  */
  YYSYMBOL_icmp6_hdr_field = 776,          /* icmp6_hdr_field  */
  YYSYMBOL_auth_hdr_expr = 777,            /* auth_hdr_expr  */
  YYSYMBOL_auth_hdr_field = 778,           /* auth_hdr_field  */
  YYSYMBOL_esp_hdr_expr = 779,             /* esp_hdr_expr  */
  YYSYMBOL_esp_hdr_field = 780,            /* esp_hdr_field  */
  YYSYMBOL_comp_hdr_expr = 781,            /* comp_hdr_expr  */
  YYSYMBOL_comp_hdr_field = 782,           /* comp_hdr_field  */
  YYSYMBOL_udp_hdr_expr = 783,             /* udp_hdr_expr  */
  YYSYMBOL_udp_hdr_field = 784,            /* udp_hdr_field  */
  YYSYMBOL_udplite_hdr_expr = 785,         /* udplite_hdr_expr  */
  YYSYMBOL_udplite_hdr_field = 786,        /* udplite_hdr_field  */
  YYSYMBOL_tcp_hdr_expr = 787,             /* tcp_hdr_expr  */
  YYSYMBOL_inner_inet_expr = 788,          /* inner_inet_expr  */
  YYSYMBOL_inner_eth_expr = 789,           /* inner_eth_expr  */
  YYSYMBOL_inner_expr = 790,               /* inner_expr  */
  YYSYMBOL_vxlan_hdr_expr = 791,           /* vxlan_hdr_expr  */
  YYSYMBOL_vxlan_hdr_field = 792,          /* vxlan_hdr_field  */
  YYSYMBOL_geneve_hdr_expr = 793,          /* geneve_hdr_expr  */
  YYSYMBOL_geneve_hdr_field = 794,         /* geneve_hdr_field  */
  YYSYMBOL_gre_hdr_expr = 795,             /* gre_hdr_expr  */
  YYSYMBOL_gre_hdr_field = 796,            /* gre_hdr_field  */
  YYSYMBOL_gretap_hdr_expr = 797,          /* gretap_hdr_expr  */
  YYSYMBOL_optstrip_stmt = 798,            /* optstrip_stmt  */
  YYSYMBOL_tcp_hdr_field = 799,            /* tcp_hdr_field  */
  YYSYMBOL_tcp_hdr_option_kind_and_field = 800, /* tcp_hdr_option_kind_and_field  */
  YYSYMBOL_tcp_hdr_option_sack = 801,      /* tcp_hdr_option_sack  */
  YYSYMBOL_tcp_hdr_option_type = 802,      /* tcp_hdr_option_type  */
  YYSYMBOL_tcpopt_field_sack = 803,        /* tcpopt_field_sack  */
  YYSYMBOL_tcpopt_field_window = 804,      /* tcpopt_field_window  */
  YYSYMBOL_tcpopt_field_tsopt = 805,       /* tcpopt_field_tsopt  */
  YYSYMBOL_tcpopt_field_maxseg = 806,      /* tcpopt_field_maxseg  */
  YYSYMBOL_tcpopt_field_mptcp = 807,       /* tcpopt_field_mptcp  */
  YYSYMBOL_dccp_hdr_expr = 808,            /* dccp_hdr_expr  */
  YYSYMBOL_dccp_hdr_field = 809,           /* dccp_hdr_field  */
  YYSYMBOL_sctp_chunk_type = 810,          /* sctp_chunk_type  */
  YYSYMBOL_sctp_chunk_common_field = 811,  /* sctp_chunk_common_field  */
  YYSYMBOL_sctp_chunk_data_field = 812,    /* sctp_chunk_data_field  */
  YYSYMBOL_sctp_chunk_init_field = 813,    /* sctp_chunk_init_field  */
  YYSYMBOL_sctp_chunk_sack_field = 814,    /* sctp_chunk_sack_field  */
  YYSYMBOL_sctp_chunk_alloc = 815,         /* sctp_chunk_alloc  */
  YYSYMBOL_sctp_hdr_expr = 816,            /* sctp_hdr_expr  */
  YYSYMBOL_sctp_hdr_field = 817,           /* sctp_hdr_field  */
  YYSYMBOL_th_hdr_expr = 818,              /* th_hdr_expr  */
  YYSYMBOL_th_hdr_field = 819,             /* th_hdr_field  */
  YYSYMBOL_exthdr_expr = 820,              /* exthdr_expr  */
  YYSYMBOL_hbh_hdr_expr = 821,             /* hbh_hdr_expr  */
  YYSYMBOL_hbh_hdr_field = 822,            /* hbh_hdr_field  */
  YYSYMBOL_rt_hdr_expr = 823,              /* rt_hdr_expr  */
  YYSYMBOL_rt_hdr_field = 824,             /* rt_hdr_field  */
  YYSYMBOL_rt0_hdr_expr = 825,             /* rt0_hdr_expr  */
  YYSYMBOL_rt0_hdr_field = 826,            /* rt0_hdr_field  */
  YYSYMBOL_rt2_hdr_expr = 827,             /* rt2_hdr_expr  */
  YYSYMBOL_rt2_hdr_field = 828,            /* rt2_hdr_field  */
  YYSYMBOL_rt4_hdr_expr = 829,             /* rt4_hdr_expr  */
  YYSYMBOL_rt4_hdr_field = 830,            /* rt4_hdr_field  */
  YYSYMBOL_frag_hdr_expr = 831,            /* frag_hdr_expr  */
  YYSYMBOL_frag_hdr_field = 832,           /* frag_hdr_field  */
  YYSYMBOL_dst_hdr_expr = 833,             /* dst_hdr_expr  */
  YYSYMBOL_dst_hdr_field = 834,            /* dst_hdr_field  */
  YYSYMBOL_mh_hdr_expr = 835,              /* mh_hdr_expr  */
  YYSYMBOL_mh_hdr_field = 836,             /* mh_hdr_field  */
  YYSYMBOL_exthdr_exists_expr = 837,       /* exthdr_exists_expr  */
  YYSYMBOL_exthdr_key = 838                /* exthdr_key  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;




#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

/* Work around bug in HP-UX 11.23, which defines these macros
   incorrectly for preprocessor constants.  This workaround can likely
   be removed in 2023, as HPE has promised support for HP-UX 11.23
   (aka HP-UX 11i v2) only through the end of 2022; see Table 2 of
   <https://h20195.www2.hpe.com/V2/getpdf.aspx/4AA4-7673ENW.pdf>.  */
#ifdef __hpux
# undef UINT_LEAST8_MAX
# undef UINT_LEAST16_MAX
# define UINT_LEAST8_MAX 255
# define UINT_LEAST16_MAX 65535
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_int16 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#if defined __GNUC__ && ! defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
# if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")
# else
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# endif
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if 1

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* 1 */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL \
             && defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
  YYLTYPE yyls_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE) \
             + YYSIZEOF (YYLTYPE)) \
      + 2 * YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  2
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   9135

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  373
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  466
/* YYNRULES -- Number of rules.  */
#define YYNRULES  1382
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  2339

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   618


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int16 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,   370,     2,     2,     2,
     367,   368,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,   364,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,   371,     2,   372,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,   365,   369,   366,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126,   127,   128,   129,   130,   131,   132,   133,   134,
     135,   136,   137,   138,   139,   140,   141,   142,   143,   144,
     145,   146,   147,   148,   149,   150,   151,   152,   153,   154,
     155,   156,   157,   158,   159,   160,   161,   162,   163,   164,
     165,   166,   167,   168,   169,   170,   171,   172,   173,   174,
     175,   176,   177,   178,   179,   180,   181,   182,   183,   184,
     185,   186,   187,   188,   189,   190,   191,   192,   193,   194,
     195,   196,   197,   198,   199,   200,   201,   202,   203,   204,
     205,   206,   207,   208,   209,   210,   211,   212,   213,   214,
     215,   216,   217,   218,   219,   220,   221,   222,   223,   224,
     225,   226,   227,   228,   229,   230,   231,   232,   233,   234,
     235,   236,   237,   238,   239,   240,   241,   242,   243,   244,
     245,   246,   247,   248,   249,   250,   251,   252,   253,   254,
     255,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,   318,   319,   320,   321,   322,   323,   324,
     325,   326,   327,   328,   329,   330,   331,   332,   333,   334,
     335,   336,   337,   338,   339,   340,   341,   342,   343,   344,
     345,   346,   347,   348,   349,   350,   351,   352,   353,   354,
     355,   356,   357,   358,   359,   360,   361,   362,   363
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,  1014,  1014,  1015,  1024,  1025,  1028,  1029,  1032,  1033,
    1034,  1035,  1036,  1037,  1038,  1039,  1040,  1041,  1042,  1043,
    1044,  1045,  1046,  1047,  1048,  1049,  1050,  1051,  1052,  1053,
    1054,  1055,  1056,  1057,  1058,  1059,  1060,  1061,  1062,  1063,
    1064,  1065,  1066,  1067,  1068,  1069,  1070,  1071,  1072,  1073,
    1074,  1075,  1076,  1077,  1078,  1079,  1080,  1081,  1082,  1084,
    1085,  1086,  1088,  1096,  1111,  1118,  1130,  1138,  1139,  1140,
    1141,  1161,  1162,  1163,  1164,  1165,  1166,  1167,  1168,  1169,
    1170,  1171,  1172,  1173,  1174,  1175,  1176,  1179,  1183,  1190,
    1194,  1202,  1206,  1210,  1217,  1224,  1234,  1241,  1250,  1254,
    1258,  1262,  1266,  1270,  1274,  1278,  1282,  1286,  1290,  1294,
    1298,  1304,  1310,  1314,  1321,  1325,  1333,  1340,  1347,  1357,
    1364,  1373,  1377,  1381,  1385,  1389,  1393,  1397,  1401,  1407,
    1413,  1414,  1417,  1418,  1421,  1422,  1425,  1426,  1429,  1433,
    1437,  1445,  1449,  1453,  1457,  1461,  1465,  1469,  1476,  1480,
    1484,  1490,  1494,  1498,  1504,  1508,  1512,  1516,  1520,  1524,
    1528,  1532,  1536,  1543,  1547,  1551,  1557,  1561,  1565,  1572,
    1578,  1579,  1581,  1582,  1585,  1589,  1593,  1597,  1601,  1605,
    1609,  1613,  1617,  1621,  1625,  1629,  1633,  1637,  1641,  1645,
    1649,  1653,  1657,  1661,  1665,  1669,  1673,  1677,  1681,  1685,
    1689,  1693,  1699,  1705,  1709,  1719,  1723,  1727,  1731,  1735,
    1739,  1743,  1747,  1752,  1756,  1760,  1764,  1770,  1774,  1778,
    1782,  1786,  1790,  1794,  1800,  1807,  1813,  1821,  1827,  1835,
    1844,  1845,  1848,  1849,  1850,  1851,  1852,  1853,  1854,  1855,
    1858,  1859,  1862,  1863,  1864,  1867,  1876,  1886,  1890,  1900,
    1901,  1906,  1920,  1921,  1922,  1923,  1924,  1935,  1945,  1956,
    1966,  1977,  1988,  1997,  2006,  2015,  2026,  2037,  2051,  2061,
    2062,  2063,  2064,  2065,  2066,  2067,  2072,  2081,  2091,  2092,
    2093,  2100,  2112,  2123,  2128,  2132,  2136,  2153,  2166,  2170,
    2183,  2188,  2189,  2192,  2193,  2194,  2195,  2205,  2210,  2215,
    2220,  2226,  2235,  2240,  2241,  2252,  2253,  2256,  2260,  2263,
    2264,  2265,  2266,  2270,  2275,  2276,  2279,  2280,  2281,  2282,
    2283,  2286,  2287,  2290,  2291,  2294,  2295,  2296,  2297,  2302,
    2307,  2324,  2347,  2361,  2370,  2375,  2381,  2386,  2395,  2398,
    2402,  2408,  2409,  2413,  2418,  2419,  2420,  2421,  2435,  2439,
    2443,  2449,  2454,  2461,  2466,  2471,  2474,  2483,  2492,  2499,
    2512,  2519,  2520,  2532,  2537,  2538,  2539,  2540,  2544,  2554,
    2555,  2556,  2557,  2561,  2571,  2572,  2573,  2574,  2578,  2589,
    2594,  2595,  2596,  2600,  2610,  2611,  2612,  2613,  2617,  2627,
    2628,  2629,  2630,  2634,  2644,  2645,  2646,  2647,  2651,  2661,
    2662,  2663,  2664,  2668,  2678,  2679,  2680,  2681,  2682,  2685,
    2721,  2728,  2732,  2735,  2745,  2752,  2763,  2776,  2791,  2792,
    2795,  2806,  2812,  2816,  2819,  2825,  2838,  2843,  2852,  2853,
    2856,  2857,  2860,  2861,  2862,  2865,  2881,  2882,  2885,  2886,
    2889,  2890,  2891,  2892,  2893,  2894,  2897,  2906,  2915,  2923,
    2931,  2939,  2947,  2955,  2963,  2971,  2979,  2987,  2995,  3003,
    3011,  3019,  3027,  3035,  3039,  3044,  3052,  3059,  3066,  3080,
    3084,  3091,  3095,  3101,  3113,  3119,  3126,  3132,  3139,  3147,
    3155,  3163,  3171,  3178,  3186,  3187,  3188,  3189,  3190,  3193,
    3194,  3195,  3196,  3197,  3200,  3201,  3202,  3203,  3204,  3205,
    3206,  3207,  3208,  3209,  3210,  3211,  3212,  3213,  3214,  3215,
    3216,  3217,  3218,  3219,  3220,  3221,  3222,  3225,  3236,  3237,
    3240,  3249,  3253,  3259,  3265,  3270,  3273,  3278,  3283,  3286,
    3292,  3298,  3301,  3307,  3316,  3317,  3319,  3325,  3329,  3332,
    3337,  3344,  3350,  3351,  3354,  3355,  3366,  3367,  3370,  3376,
    3380,  3383,  3400,  3405,  3410,  3415,  3420,  3426,  3456,  3460,
    3464,  3468,  3472,  3478,  3482,  3485,  3489,  3495,  3501,  3504,
    3522,  3537,  3538,  3539,  3542,  3543,  3546,  3547,  3562,  3568,
    3571,  3592,  3593,  3594,  3597,  3598,  3601,  3608,  3609,  3612,
    3626,  3633,  3634,  3649,  3650,  3651,  3652,  3653,  3656,  3659,
    3665,  3671,  3675,  3679,  3686,  3693,  3700,  3707,  3713,  3719,
    3725,  3728,  3729,  3732,  3738,  3744,  3750,  3757,  3764,  3772,
    3773,  3776,  3782,  3786,  3789,  3794,  3799,  3803,  3809,  3825,
    3844,  3850,  3851,  3857,  3858,  3864,  3865,  3866,  3867,  3868,
    3869,  3870,  3871,  3872,  3873,  3874,  3875,  3876,  3879,  3880,
    3884,  3890,  3891,  3897,  3898,  3904,  3905,  3911,  3914,  3915,
    3926,  3927,  3930,  3934,  3937,  3943,  3949,  3950,  3953,  3954,
    3955,  3958,  3962,  3966,  3971,  3976,  3981,  3987,  3991,  3995,
    3999,  4005,  4010,  4014,  4022,  4031,  4032,  4035,  4038,  4042,
    4047,  4053,  4054,  4057,  4060,  4064,  4068,  4072,  4077,  4084,
    4089,  4097,  4102,  4111,  4112,  4118,  4119,  4120,  4123,  4124,
    4128,  4132,  4138,  4139,  4142,  4148,  4152,  4155,  4160,  4166,
    4167,  4170,  4171,  4172,  4178,  4179,  4180,  4181,  4184,  4185,
    4191,  4192,  4195,  4196,  4199,  4205,  4212,  4219,  4230,  4231,
    4232,  4235,  4243,  4255,  4264,  4275,  4281,  4307,  4308,  4317,
    4318,  4321,  4330,  4341,  4342,  4343,  4344,  4345,  4346,  4347,
    4348,  4349,  4350,  4351,  4352,  4355,  4356,  4357,  4358,  4361,
    4384,  4385,  4386,  4389,  4390,  4391,  4392,  4393,  4396,  4400,
    4403,  4407,  4414,  4417,  4433,  4434,  4438,  4444,  4445,  4451,
    4452,  4458,  4459,  4465,  4468,  4469,  4480,  4486,  4501,  4502,
    4505,  4511,  4512,  4513,  4516,  4523,  4528,  4533,  4536,  4540,
    4544,  4550,  4551,  4558,  4564,  4565,  4566,  4574,  4575,  4578,
    4584,  4590,  4594,  4597,  4618,  4622,  4626,  4636,  4640,  4643,
    4649,  4656,  4657,  4658,  4659,  4660,  4663,  4667,  4671,  4681,
    4684,  4685,  4688,  4689,  4690,  4691,  4702,  4713,  4719,  4740,
    4746,  4763,  4769,  4770,  4771,  4774,  4775,  4776,  4779,  4780,
    4783,  4806,  4812,  4818,  4825,  4838,  4846,  4854,  4860,  4864,
    4868,  4872,  4876,  4883,  4888,  4899,  4913,  4919,  4923,  4927,
    4934,  4942,  4949,  4957,  4961,  4967,  4973,  4981,  4982,  4983,
    4986,  4987,  4991,  4997,  4998,  5004,  5005,  5011,  5012,  5018,
    5021,  5022,  5023,  5032,  5043,  5044,  5047,  5055,  5056,  5057,
    5058,  5059,  5060,  5061,  5062,  5063,  5064,  5065,  5066,  5067,
    5068,  5071,  5072,  5073,  5074,  5075,  5082,  5089,  5096,  5103,
    5110,  5117,  5124,  5131,  5138,  5145,  5152,  5159,  5166,  5169,
    5170,  5171,  5172,  5173,  5174,  5175,  5178,  5182,  5186,  5190,
    5194,  5198,  5204,  5205,  5215,  5219,  5223,  5239,  5240,  5243,
    5244,  5245,  5246,  5247,  5250,  5251,  5252,  5253,  5254,  5255,
    5256,  5257,  5258,  5259,  5260,  5261,  5262,  5263,  5264,  5265,
    5266,  5267,  5268,  5269,  5270,  5271,  5272,  5273,  5276,  5296,
    5300,  5315,  5319,  5323,  5329,  5333,  5339,  5340,  5341,  5344,
    5345,  5348,  5349,  5352,  5358,  5359,  5362,  5363,  5366,  5367,
    5370,  5371,  5374,  5382,  5409,  5414,  5419,  5425,  5426,  5429,
    5433,  5453,  5454,  5455,  5456,  5459,  5463,  5467,  5473,  5474,
    5477,  5478,  5479,  5480,  5481,  5482,  5483,  5484,  5485,  5486,
    5487,  5488,  5489,  5490,  5491,  5492,  5493,  5496,  5497,  5498,
    5499,  5500,  5501,  5502,  5505,  5506,  5507,  5508,  5511,  5512,
    5513,  5514,  5517,  5518,  5521,  5527,  5535,  5548,  5554,  5563,
    5564,  5565,  5566,  5567,  5568,  5569,  5570,  5571,  5572,  5573,
    5574,  5575,  5576,  5577,  5578,  5579,  5580,  5581,  5582,  5583,
    5584,  5587,  5605,  5614,  5615,  5616,  5617,  5630,  5636,  5637,
    5638,  5641,  5647,  5648,  5649,  5650,  5651,  5654,  5660,  5661,
    5662,  5663,  5664,  5665,  5666,  5667,  5668,  5671,  5675,  5686,
    5693,  5694,  5695,  5696,  5697,  5698,  5699,  5700,  5701,  5702,
    5703,  5704,  5707,  5708,  5709,  5710,  5713,  5714,  5715,  5716,
    5717,  5720,  5726,  5727,  5728,  5729,  5730,  5731,  5732,  5735,
    5741,  5742,  5743,  5744,  5747,  5753,  5754,  5755,  5756,  5757,
    5758,  5759,  5760,  5761,  5763,  5769,  5770,  5771,  5772,  5773,
    5774,  5775,  5776,  5777,  5778,  5781,  5787,  5788,  5789,  5790,
    5791,  5794,  5800,  5801,  5804,  5810,  5811,  5812,  5815,  5821,
    5822,  5823,  5824,  5827,  5833,  5834,  5835,  5836,  5839,  5843,
    5848,  5856,  5863,  5864,  5865,  5866,  5867,  5868,  5869,  5870,
    5871,  5872,  5873,  5874,  5875,  5876,  5879,  5880,  5881,  5884,
    5885,  5888,  5896,  5904,  5905,  5908,  5916,  5924,  5925,  5928,
    5932,  5939,  5940,  5941,  5944,  5951,  5958,  5959,  5960,  5961,
    5962,  5963,  5964,  5965,  5966,  5967,  5970,  5975,  5980,  5985,
    5990,  5995,  6002,  6003,  6004,  6005,  6006,  6009,  6010,  6011,
    6012,  6013,  6014,  6015,  6016,  6017,  6018,  6019,  6020,  6029,
    6030,  6033,  6036,  6037,  6040,  6043,  6046,  6050,  6061,  6062,
    6063,  6066,  6067,  6068,  6069,  6070,  6071,  6072,  6073,  6074,
    6075,  6076,  6077,  6078,  6079,  6080,  6081,  6082,  6083,  6086,
    6087,  6088,  6091,  6092,  6093,  6094,  6097,  6098,  6099,  6100,
    6101,  6104,  6105,  6106,  6107,  6110,  6115,  6119,  6123,  6127,
    6131,  6135,  6140,  6145,  6150,  6155,  6160,  6167,  6171,  6177,
    6178,  6179,  6180,  6183,  6191,  6192,  6195,  6196,  6197,  6198,
    6199,  6200,  6201,  6202,  6205,  6211,  6212,  6215,  6221,  6222,
    6223,  6224,  6227,  6233,  6239,  6245,  6248,  6254,  6255,  6256,
    6257,  6263,  6269,  6270,  6271,  6272,  6273,  6274,  6277,  6283,
    6284,  6287,  6293,  6294,  6295,  6296,  6297,  6300,  6314,  6315,
    6316,  6317,  6318
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if 1
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "\"junk\"",
  "\"CRLF line terminators\"", "\"newline\"", "\"colon\"", "\"semicolon\"",
  "\"comma\"", "\".\"", "\"==\"", "\"!=\"", "\"<\"", "\">\"", "\">=\"",
  "\"<=\"", "\"<<\"", "\">>\"", "\"&\"", "\"^\"", "\"!\"", "\"/\"",
  "\"*\"", "\"-\"", "\"@\"", "\"vmap\"", "\"+\"", "\"include\"",
  "\"define\"", "\"redefine\"", "\"undefine\"", "\"fib\"", "\"socket\"",
  "\"transparent\"", "\"wildcard\"", "\"cgroupv2\"", "\"tproxy\"",
  "\"osf\"", "\"synproxy\"", "\"mss\"", "\"wscale\"", "\"typeof\"",
  "\"hook\"", "\"hooks\"", "\"device\"", "\"devices\"", "\"table\"",
  "\"tables\"", "\"chain\"", "\"chains\"", "\"rule\"", "\"rules\"",
  "\"sets\"", "\"set\"", "\"element\"", "\"map\"", "\"maps\"",
  "\"flowtable\"", "\"handle\"", "\"ruleset\"", "\"trace\"", "\"inet\"",
  "\"netdev\"", "\"add\"", "\"update\"", "\"replace\"", "\"create\"",
  "\"insert\"", "\"delete\"", "\"get\"", "\"list\"", "\"reset\"",
  "\"flush\"", "\"rename\"", "\"describe\"", "\"import\"", "\"export\"",
  "\"destroy\"", "\"monitor\"", "\"all\"", "\"accept\"", "\"drop\"",
  "\"continue\"", "\"jump\"", "\"goto\"", "\"return\"", "\"to\"",
  "\"constant\"", "\"interval\"", "\"dynamic\"", "\"auto-merge\"",
  "\"timeout\"", "\"gc-interval\"", "\"elements\"", "\"expires\"",
  "\"policy\"", "\"memory\"", "\"performance\"", "\"size\"", "\"flow\"",
  "\"offload\"", "\"meter\"", "\"meters\"", "\"flowtables\"", "\"number\"",
  "\"string\"", "\"quoted string\"", "\"string with a trailing asterisk\"",
  "\"ll\"", "\"nh\"", "\"th\"", "\"bridge\"", "\"ether\"", "\"saddr\"",
  "\"daddr\"", "\"type\"", "\"vlan\"", "\"id\"", "\"cfi\"", "\"dei\"",
  "\"pcp\"", "\"arp\"", "\"htype\"", "\"ptype\"", "\"hlen\"", "\"plen\"",
  "\"operation\"", "\"ip\"", "\"version\"", "\"hdrlength\"", "\"dscp\"",
  "\"ecn\"", "\"length\"", "\"frag-off\"", "\"ttl\"", "\"protocol\"",
  "\"checksum\"", "\"ptr\"", "\"value\"", "\"lsrr\"", "\"rr\"", "\"ssrr\"",
  "\"ra\"", "\"icmp\"", "\"code\"", "\"seq\"", "\"gateway\"", "\"mtu\"",
  "\"igmp\"", "\"mrt\"", "\"options\"", "\"ip6\"", "\"priority\"",
  "\"flowlabel\"", "\"nexthdr\"", "\"hoplimit\"", "\"icmpv6\"",
  "\"param-problem\"", "\"max-delay\"", "\"taddr\"", "\"ah\"",
  "\"reserved\"", "\"spi\"", "\"esp\"", "\"comp\"", "\"flags\"", "\"cpi\"",
  "\"port\"", "\"udp\"", "\"sport\"", "\"dport\"", "\"udplite\"",
  "\"csumcov\"", "\"tcp\"", "\"ackseq\"", "\"doff\"", "\"window\"",
  "\"urgptr\"", "\"option\"", "\"echo\"", "\"eol\"", "\"mptcp\"",
  "\"nop\"", "\"sack\"", "\"sack0\"", "\"sack1\"", "\"sack2\"",
  "\"sack3\"", "\"sack-permitted\"", "\"fastopen\"", "\"md5sig\"",
  "\"timestamp\"", "\"count\"", "\"left\"", "\"right\"", "\"tsval\"",
  "\"tsecr\"", "\"subtype\"", "\"dccp\"", "\"vxlan\"", "\"vni\"",
  "\"gre\"", "\"gretap\"", "\"geneve\"", "\"sctp\"", "\"chunk\"",
  "\"data\"", "\"init\"", "\"init-ack\"", "\"heartbeat\"",
  "\"heartbeat-ack\"", "\"abort\"", "\"shutdown\"", "\"shutdown-ack\"",
  "\"error\"", "\"cookie-echo\"", "\"cookie-ack\"", "\"ecne\"", "\"cwr\"",
  "\"shutdown-complete\"", "\"asconf-ack\"", "\"forward-tsn\"",
  "\"asconf\"", "\"tsn\"", "\"stream\"", "\"ssn\"", "\"ppid\"",
  "\"init-tag\"", "\"a-rwnd\"", "\"num-outbound-streams\"",
  "\"num-inbound-streams\"", "\"initial-tsn\"", "\"cum-tsn-ack\"",
  "\"num-gap-ack-blocks\"", "\"num-dup-tsns\"", "\"lowest-tsn\"",
  "\"seqno\"", "\"new-cum-tsn\"", "\"vtag\"", "\"rt\"", "\"rt0\"",
  "\"rt2\"", "\"srh\"", "\"seg-left\"", "\"addr\"", "\"last-entry\"",
  "\"tag\"", "\"sid\"", "\"hbh\"", "\"frag\"", "\"reserved2\"",
  "\"more-fragments\"", "\"dst\"", "\"mh\"", "\"meta\"", "\"mark\"",
  "\"iif\"", "\"iifname\"", "\"iiftype\"", "\"oif\"", "\"oifname\"",
  "\"oiftype\"", "\"skuid\"", "\"skgid\"", "\"nftrace\"", "\"rtclassid\"",
  "\"ibriport\"", "\"obriport\"", "\"ibrname\"", "\"obrname\"",
  "\"pkttype\"", "\"cpu\"", "\"iifgroup\"", "\"oifgroup\"", "\"cgroup\"",
  "\"time\"", "\"classid\"", "\"nexthop\"", "\"ct\"", "\"l3proto\"",
  "\"proto-src\"", "\"proto-dst\"", "\"zone\"", "\"direction\"",
  "\"event\"", "\"expectation\"", "\"expiration\"", "\"helper\"",
  "\"label\"", "\"state\"", "\"status\"", "\"original\"", "\"reply\"",
  "\"counter\"", "\"name\"", "\"packets\"", "\"bytes\"", "\"avgpkt\"",
  "\"last\"", "\"never\"", "\"counters\"", "\"quotas\"", "\"limits\"",
  "\"synproxys\"", "\"helpers\"", "\"log\"", "\"prefix\"", "\"group\"",
  "\"snaplen\"", "\"queue-threshold\"", "\"level\"", "\"limit\"",
  "\"rate\"", "\"burst\"", "\"over\"", "\"until\"", "\"quota\"",
  "\"used\"", "\"secmark\"", "\"secmarks\"", "\"second\"", "\"minute\"",
  "\"hour\"", "\"day\"", "\"week\"", "\"reject\"", "\"with\"", "\"icmpx\"",
  "\"snat\"", "\"dnat\"", "\"masquerade\"", "\"redirect\"", "\"random\"",
  "\"fully-random\"", "\"persistent\"", "\"queue\"", "\"num\"",
  "\"bypass\"", "\"fanout\"", "\"dup\"", "\"fwd\"", "\"numgen\"",
  "\"inc\"", "\"mod\"", "\"offset\"", "\"jhash\"", "\"symhash\"",
  "\"seed\"", "\"position\"", "\"index\"", "\"comment\"", "\"xml\"",
  "\"json\"", "\"vm\"", "\"notrack\"", "\"exists\"", "\"missing\"",
  "\"exthdr\"", "\"ipsec\"", "\"reqid\"", "\"spnum\"", "\"in\"", "\"out\"",
  "\"xt\"", "'='", "'{'", "'}'", "'('", "')'", "'|'", "'$'", "'['", "']'",
  "$accept", "input", "stmt_separator", "opt_newline", "close_scope_ah",
  "close_scope_arp", "close_scope_at", "close_scope_comp",
  "close_scope_ct", "close_scope_counter", "close_scope_last",
  "close_scope_dccp", "close_scope_destroy", "close_scope_dst",
  "close_scope_dup", "close_scope_esp", "close_scope_eth",
  "close_scope_export", "close_scope_fib", "close_scope_frag",
  "close_scope_fwd", "close_scope_gre", "close_scope_hash",
  "close_scope_hbh", "close_scope_ip", "close_scope_ip6",
  "close_scope_vlan", "close_scope_icmp", "close_scope_igmp",
  "close_scope_import", "close_scope_ipsec", "close_scope_list",
  "close_scope_limit", "close_scope_meta", "close_scope_mh",
  "close_scope_monitor", "close_scope_nat", "close_scope_numgen",
  "close_scope_osf", "close_scope_policy", "close_scope_quota",
  "close_scope_queue", "close_scope_reject", "close_scope_reset",
  "close_scope_rt", "close_scope_sctp", "close_scope_sctp_chunk",
  "close_scope_secmark", "close_scope_socket", "close_scope_tcp",
  "close_scope_tproxy", "close_scope_type", "close_scope_th",
  "close_scope_udp", "close_scope_udplite", "close_scope_log",
  "close_scope_synproxy", "close_scope_xt", "common_block", "line",
  "base_cmd", "add_cmd", "replace_cmd", "create_cmd", "insert_cmd",
  "table_or_id_spec", "chain_or_id_spec", "set_or_id_spec",
  "obj_or_id_spec", "delete_cmd", "destroy_cmd", "get_cmd",
  "list_cmd_spec_table", "list_cmd_spec_any", "list_cmd",
  "basehook_device_name", "basehook_spec", "reset_cmd", "flush_cmd",
  "rename_cmd", "import_cmd", "export_cmd", "monitor_cmd", "monitor_event",
  "monitor_object", "monitor_format", "markup_format", "describe_cmd",
  "table_block_alloc", "table_options", "table_flags", "table_flag",
  "table_block", "chain_block_alloc", "chain_block", "subchain_block",
  "typeof_verdict_expr", "typeof_data_expr", "primary_typeof_expr",
  "typeof_expr", "set_block_alloc", "typeof_key_expr", "set_block",
  "set_block_expr", "set_flag_list", "set_flag", "map_block_alloc",
  "ct_obj_type_map", "map_block_obj_type", "map_block_obj_typeof",
  "map_block_data_interval", "map_block", "set_mechanism",
  "set_policy_spec", "flowtable_block_alloc", "flowtable_block",
  "flowtable_expr", "flowtable_list_expr", "flowtable_expr_member",
  "data_type_atom_expr", "data_type_expr", "obj_block_alloc",
  "counter_block", "quota_block", "ct_helper_block", "ct_timeout_block",
  "ct_expect_block", "limit_block", "secmark_block", "synproxy_block",
  "type_identifier", "hook_spec", "prio_spec", "extended_prio_name",
  "extended_prio_spec", "int_num", "dev_spec", "flags_spec", "policy_spec",
  "policy_expr", "chain_policy", "identifier", "string", "time_spec",
  "time_spec_or_num_s", "family_spec", "family_spec_explicit",
  "table_spec", "tableid_spec", "chain_spec", "chainid_spec",
  "chain_identifier", "set_spec", "setid_spec", "set_identifier",
  "flowtable_spec", "flowtableid_spec", "flowtable_identifier", "obj_spec",
  "objid_spec", "obj_identifier", "handle_spec", "position_spec",
  "index_spec", "rule_position", "ruleid_spec", "comment_spec",
  "ruleset_spec", "rule", "rule_alloc", "stmt_list", "stateful_stmt_list",
  "objref_stmt_counter", "objref_stmt_limit", "objref_stmt_quota",
  "objref_stmt_synproxy", "objref_stmt_ct", "objref_stmt", "stateful_stmt",
  "stmt", "xt_stmt", "chain_stmt_type", "chain_stmt", "verdict_stmt",
  "verdict_map_stmt", "verdict_map_expr", "verdict_map_list_expr",
  "verdict_map_list_member_expr", "ct_limit_stmt_alloc", "connlimit_stmt",
  "ct_limit_args", "counter_stmt", "counter_stmt_alloc", "counter_args",
  "counter_arg", "last_stmt_alloc", "last_stmt", "last_args", "log_stmt",
  "log_stmt_alloc", "log_args", "log_arg", "level_type", "log_flags",
  "log_flags_tcp", "log_flag_tcp", "limit_stmt_alloc", "limit_stmt",
  "limit_args", "quota_mode", "quota_unit", "quota_used",
  "quota_stmt_alloc", "quota_stmt", "quota_args", "limit_mode",
  "limit_burst_pkts", "limit_rate_pkts", "limit_burst_bytes",
  "limit_rate_bytes", "limit_bytes", "time_unit", "reject_stmt",
  "reject_stmt_alloc", "reject_with_expr", "reject_opts", "nat_stmt",
  "nat_stmt_alloc", "tproxy_stmt", "synproxy_stmt", "synproxy_stmt_alloc",
  "synproxy_args", "synproxy_arg", "synproxy_config", "synproxy_obj",
  "synproxy_ts", "synproxy_sack", "primary_stmt_expr", "shift_stmt_expr",
  "and_stmt_expr", "exclusive_or_stmt_expr", "inclusive_or_stmt_expr",
  "basic_stmt_expr", "concat_stmt_expr", "map_stmt_expr_set",
  "map_stmt_expr", "prefix_stmt_expr", "range_stmt_expr",
  "multiton_stmt_expr", "stmt_expr", "nat_stmt_args", "masq_stmt",
  "masq_stmt_alloc", "masq_stmt_args", "redir_stmt", "redir_stmt_alloc",
  "redir_stmt_arg", "dup_stmt", "fwd_stmt", "nf_nat_flags", "nf_nat_flag",
  "queue_stmt", "queue_stmt_compat", "queue_stmt_alloc", "queue_stmt_args",
  "queue_stmt_arg", "queue_expr", "queue_stmt_expr_simple",
  "queue_stmt_expr", "queue_stmt_flags", "queue_stmt_flag",
  "set_elem_expr_stmt", "set_elem_expr_stmt_alloc", "set_stmt",
  "set_stmt_op", "map_stmt", "meter_stmt", "match_stmt", "variable_expr",
  "symbol_expr", "set_ref_expr", "set_ref_symbol_expr", "integer_expr",
  "selector_expr", "primary_expr", "fib_expr", "fib_result", "fib_flag",
  "fib_tuple", "osf_expr", "osf_ttl", "shift_expr", "and_expr",
  "exclusive_or_expr", "inclusive_or_expr", "basic_expr", "concat_expr",
  "prefix_rhs_expr", "range_rhs_expr", "multiton_rhs_expr", "map_expr",
  "expr", "set_expr", "set_list_expr", "set_list_member_expr",
  "meter_key_expr", "meter_key_expr_alloc", "set_elem_expr",
  "set_elem_key_expr", "set_elem_expr_alloc", "set_elem_options",
  "set_elem_time_spec", "set_elem_option", "set_elem_expr_options",
  "set_elem_stmt_list", "set_elem_stmt", "set_elem_expr_option",
  "set_lhs_expr", "set_rhs_expr", "initializer_expr", "counter_config",
  "counter_obj", "quota_config", "quota_obj", "secmark_config",
  "secmark_obj", "ct_obj_type", "ct_cmd_type", "ct_l4protoname",
  "ct_helper_config", "timeout_states", "timeout_state",
  "ct_timeout_config", "ct_expect_config", "ct_obj_alloc", "limit_config",
  "limit_obj", "relational_expr", "list_rhs_expr", "rhs_expr",
  "shift_rhs_expr", "and_rhs_expr", "exclusive_or_rhs_expr",
  "inclusive_or_rhs_expr", "basic_rhs_expr", "concat_rhs_expr",
  "boolean_keys", "boolean_expr", "keyword_expr", "primary_rhs_expr",
  "relational_op", "verdict_expr", "chain_expr", "meta_expr", "meta_key",
  "meta_key_qualified", "meta_key_unqualified", "meta_stmt", "socket_expr",
  "socket_key", "offset_opt", "numgen_type", "numgen_expr", "xfrm_spnum",
  "xfrm_dir", "xfrm_state_key", "xfrm_state_proto_key", "xfrm_expr",
  "hash_expr", "nf_key_proto", "rt_expr", "rt_key", "ct_expr", "ct_dir",
  "ct_key", "ct_key_dir", "ct_key_proto_field", "ct_key_dir_optional",
  "symbol_stmt_expr", "list_stmt_expr", "ct_stmt", "payload_stmt",
  "payload_expr", "payload_raw_len", "payload_raw_expr",
  "payload_base_spec", "eth_hdr_expr", "eth_hdr_field", "vlan_hdr_expr",
  "vlan_hdr_field", "arp_hdr_expr", "arp_hdr_field", "ip_hdr_expr",
  "ip_hdr_field", "ip_option_type", "ip_option_field", "icmp_hdr_expr",
  "icmp_hdr_field", "igmp_hdr_expr", "igmp_hdr_field", "ip6_hdr_expr",
  "ip6_hdr_field", "icmp6_hdr_expr", "icmp6_hdr_field", "auth_hdr_expr",
  "auth_hdr_field", "esp_hdr_expr", "esp_hdr_field", "comp_hdr_expr",
  "comp_hdr_field", "udp_hdr_expr", "udp_hdr_field", "udplite_hdr_expr",
  "udplite_hdr_field", "tcp_hdr_expr", "inner_inet_expr", "inner_eth_expr",
  "inner_expr", "vxlan_hdr_expr", "vxlan_hdr_field", "geneve_hdr_expr",
  "geneve_hdr_field", "gre_hdr_expr", "gre_hdr_field", "gretap_hdr_expr",
  "optstrip_stmt", "tcp_hdr_field", "tcp_hdr_option_kind_and_field",
  "tcp_hdr_option_sack", "tcp_hdr_option_type", "tcpopt_field_sack",
  "tcpopt_field_window", "tcpopt_field_tsopt", "tcpopt_field_maxseg",
  "tcpopt_field_mptcp", "dccp_hdr_expr", "dccp_hdr_field",
  "sctp_chunk_type", "sctp_chunk_common_field", "sctp_chunk_data_field",
  "sctp_chunk_init_field", "sctp_chunk_sack_field", "sctp_chunk_alloc",
  "sctp_hdr_expr", "sctp_hdr_field", "th_hdr_expr", "th_hdr_field",
  "exthdr_expr", "hbh_hdr_expr", "hbh_hdr_field", "rt_hdr_expr",
  "rt_hdr_field", "rt0_hdr_expr", "rt0_hdr_field", "rt2_hdr_expr",
  "rt2_hdr_field", "rt4_hdr_expr", "rt4_hdr_field", "frag_hdr_expr",
  "frag_hdr_field", "dst_hdr_expr", "dst_hdr_field", "mh_hdr_expr",
  "mh_hdr_field", "exthdr_exists_expr", "exthdr_key", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-1824)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-1064)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
   -1824,  8483, -1824,   394, -1824, -1824,   187,   309,   309,   309,
    1266,  1266,  1266,  1266,  1266,  1266,  1266,  1266, -1824, -1824,
    3274,   247,  2872,   297,   943,   310,  3577,   912,  1445,   323,
    7650,   250,   318,  3483,   278, -1824, -1824, -1824, -1824,   334,
    1266,  1266,  1266,  1266, -1824, -1824, -1824,  1050, -1824,   309,
   -1824,   309,   144,  6998, -1824,   394, -1824, -1824,    78,    84,
     394,   309, -1824,   124,   262,  6998,   309, -1824,  -123, -1824,
     309, -1824, -1824,  1266, -1824,  1266,  1266,  1266,  1266,  1266,
    1266,  1266,   517,  1266,  1266,  1266,  1266, -1824,  1266, -1824,
    1266,  1266,  1266,  1266,  1266,  1266,  1266,  1266,   601,  1266,
    1266,  1266,  1266, -1824,  1266, -1824,  1266,  1266,  1266,  1266,
    1266,  1266,  2008,  1266,  1266,  2008,  1266,  1266,   683,  1266,
    1266,  2008,   493,  1266,  2008,  2008,  2008,  2008,  1266,  1266,
    1266,  2008, -1824,  1266,  2268,  1266,  1266,  1266,  1266,  2008,
    2008,  1266, -1824,  1266,  1266,  1266,  1266,  1266,   587,  1266,
   -1824,  1266, -1824,  1205,   924,   269,   539, -1824, -1824, -1824,
   -1824,   657,  1251,  1089,  1787,  2570,  1645,   191,  2606,  2595,
    1265,   199,   819,   486,   844,  1455,   767,  5773,   188, -1824,
    5982,   870,  1561,   415,   507,   971,   630,  1269,   640,  1017,
    3975, -1824, -1824, -1824, -1824, -1824, -1824, -1824, -1824, -1824,
   -1824, -1824, -1824, -1824, -1824, -1824, -1824, -1824, -1824, -1824,
   -1824, -1824,  5239, -1824, -1824,   490,  7474,   428,  1029,   511,
    7650,   309, -1824, -1824, -1824, -1824, -1824, -1824, -1824, -1824,
   -1824, -1824, -1824, -1824, -1824, -1824, -1824, -1824, -1824, -1824,
   -1824, -1824, -1824, -1824, -1824, -1824, -1824, -1824, -1824, -1824,
   -1824, -1824, -1824, -1824, -1824, -1824, -1824, -1824, -1824, -1824,
   -1824, -1824, -1824, -1824, -1824, -1824, -1824, -1824, -1824, -1824,
   -1824, -1824,  1113, -1824, -1824,   459, -1824, -1824,  1113, -1824,
   -1824,  1266,  1266,  1266,  1266,  1266,  1266,  1266,  1266,   601,
    1266,  1266,  1266,  1266, -1824, -1824, -1824,  1687, -1824, -1824,
   -1824,  1266,  1266,  1266,    -7, -1824, -1824, -1824, -1824, -1824,
   -1824, -1824,   715,   729,   756, -1824, -1824, -1824,   615,   562,
    1254, -1824, -1824, -1824,   691, -1824, -1824, -1824,   153,   153,
   -1824,   273,   309,  5027,  4278,   603, -1824, -1824,    63,   612,
   -1824, -1824, -1824, -1824, -1824,   238,   836,   689, -1824,   805,
     929, -1824,   579,  6998, -1824, -1824, -1824, -1824, -1824, -1824,
   -1824, -1824, -1824, -1824, -1824, -1824, -1824,   136, -1824, -1824,
     659,   633, -1824, -1824,   796,   737, -1824,   752, -1824, -1824,
     653, -1824,  5341, -1824, -1824,   753, -1824,   141, -1824,   377,
   -1824, -1824, -1824, -1824,  1169, -1824,   171, -1824, -1824, -1824,
   -1824,  1221,   950,   964,   619, -1824,   892, -1824,  6386, -1824,
   -1824, -1824,   942, -1824, -1824, -1824,   965, -1824, -1824,  6767,
    6767, -1824, -1824,   146,   667,   676, -1824, -1824,   709, -1824,
   -1824, -1824,   716, -1824,   727,  1040,  6998, -1824,   124,   262,
   -1824,  -123, -1824, -1824,  1266,  1266,  1266,   812, -1824, -1824,
   -1824,  6998, -1824,   277, -1824, -1824, -1824,   292, -1824, -1824,
   -1824,   331,   262, -1824, -1824, -1824,   567, -1824, -1824,  -123,
   -1824,   642,   754, -1824, -1824, -1824, -1824,  1266, -1824, -1824,
   -1824, -1824,  -123, -1824, -1824, -1824,  1073, -1824, -1824, -1824,
   -1824,  1266, -1824, -1824,   359, -1824, -1824, -1824, -1824, -1824,
   -1824, -1824,  1266,  1266, -1824, -1824, -1824,  1091,  1101, -1824,
    1266,  1112, -1824, -1824, -1824, -1824, -1824, -1824, -1824, -1824,
   -1824, -1824, -1824,  1266, -1824,   309, -1824, -1824, -1824,  -123,
   -1824, -1824, -1824, -1824, -1824, -1824, -1824, -1824, -1824, -1824,
   -1824,  1266, -1824,   309, -1824, -1824, -1824, -1824,  1116, -1824,
   -1824, -1824, -1824, -1824,  1121,   244, -1824, -1824,   879, -1824,
   -1824,  1086,   118, -1824, -1824, -1824, -1824, -1824, -1824, -1824,
   -1824, -1824, -1824, -1824, -1824, -1824,   303,   669, -1824, -1824,
   -1824, -1824, -1824, -1824, -1824, -1824, -1824, -1824, -1824, -1824,
   -1824, -1824, -1824, -1824, -1824, -1824,  1346, -1824, -1824, -1824,
   -1824, -1824, -1824, -1824, -1824, -1824, -1824, -1824, -1824, -1824,
   -1824, -1824, -1824, -1824, -1824, -1824, -1824, -1824, -1824, -1824,
   -1824, -1824, -1824, -1824, -1824, -1824, -1824, -1824, -1824, -1824,
   -1824, -1824, -1824, -1824, -1824, -1824, -1824, -1824, -1824, -1824,
   -1824, -1824, -1824, -1824, -1824, -1824, -1824, -1824, -1824, -1824,
   -1824, -1824, -1824, -1824, -1824, -1824, -1824, -1824, -1824, -1824,
   -1824, -1824, -1824, -1824, -1824,  2988, -1824, -1824, -1824, -1824,
    1095, -1824, -1824, -1824, -1824, -1824, -1824, -1824, -1824, -1824,
   -1824, -1824, -1824, -1824, -1824, -1824, -1824, -1824, -1824, -1824,
   -1824, -1824, -1824, -1824, -1824, -1824, -1824, -1824,  6461, -1824,
    6191, -1824, -1824, -1824, -1824, -1824, -1824, -1824,  3982, -1824,
   -1824, -1824, -1824, -1824, -1824, -1824, -1824, -1824, -1824, -1824,
   -1824,   581, -1824, -1824,   840, -1824, -1824, -1824, -1824, -1824,
   -1824,   856, -1824, -1824, -1824, -1824, -1824, -1824, -1824, -1824,
   -1824, -1824, -1824, -1824, -1824, -1824, -1824, -1824, -1824, -1824,
   -1824, -1824, -1824, -1824, -1824, -1824, -1824, -1824, -1824, -1824,
   -1824, -1824, -1824, -1824, -1824, -1824, -1824, -1824, -1824, -1824,
   -1824, -1824, -1824, -1824, -1824, -1824, -1824, -1824, -1824, -1824,
   -1824, -1824, -1824, -1824,  1547, -1824, -1824, -1824, -1824,   872,
     252,   889,  1149, -1824, -1824, -1824, -1824, -1824, -1824, -1824,
   -1824, -1824,   895,   915, -1824, -1824, -1824, -1824, -1824, -1824,
   -1824, -1824, -1824, -1824, -1824, -1824, -1824,  -123, -1824,   754,
   -1824,  1266, -1824, -1824, -1824, -1824, -1824, -1824, -1824, -1824,
   -1824, -1824, -1824, -1824, -1824,  1113, -1824, -1824, -1824, -1824,
     282,     0,   398,   279, -1824, -1824, -1824,  5550,  1226,  7265,
    7650,  1147, -1824, -1824, -1824, -1824,  1305,  1311,   135,  1284,
    1293,  1317, -1824,  1343,  1547,  1355,  7265,  7265, -1824,  7265,
    7650,   928,  7265,  7265,  1326,  1407, -1824,  6487,   174, -1824,
    1407, -1824, -1824, -1824,  1057, -1824,  1321, -1824, -1824, -1824,
    1324,  1353,   659, -1824,   173, -1824, -1824, -1824,   780,  1407,
    1364,  1376,  1386,  1407,   796, -1824, -1824, -1824, -1824,  1388,
   -1824, -1824, -1824,  1390, -1824, -1824, -1824,   814, -1824, -1824,
    7265, -1824, -1824,  5759,  1345,  1251,  1089,  1787,  2570, -1824,
    2606,   963, -1824, -1824, -1824, -1824,  1362, -1824, -1824, -1824,
   -1824,  7265, -1824,  1263,  1432,  1480,  1132,   978,   325, -1824,
   -1824, -1824, -1824,  1503,  1216,  1508, -1824, -1824, -1824, -1824,
    1514, -1824, -1824, -1824, -1824,   672, -1824, -1824,  1520,  1531,
   -1824, -1824, -1824,  1447,  1449, -1824, -1824,   753, -1824, -1824,
    1554, -1824, -1824, -1824, -1824,  1557, -1824, -1824,  5968, -1824,
    1557, -1824, -1824, -1824,   120, -1824, -1824,  1169, -1824,  1563,
   -1824,   309, -1824,  1208, -1824,  8576,  8576,  8576,  8576,  8576,
    7650,   151,  7855, -1824, -1824, -1824, -1824, -1824, -1824, -1824,
   -1824, -1824, -1824, -1824, -1824, -1824, -1824, -1824, -1824, -1824,
   -1824, -1824, -1824, -1824, -1824, -1824, -1824, -1824,  8576, -1824,
   -1824, -1824, -1824, -1824, -1824, -1824,   399, -1824,  1303,  1565,
    1560,  1215,  1092,  1577, -1824, -1824, -1824,  7855,  7265,  7265,
    1485,   156,   394,  1587, -1824,  1154,   394,  1495, -1824, -1824,
   -1824, -1824, -1824, -1824, -1824, -1824, -1824,  1566,  1239,  1241,
    1243, -1824,  1245,  1246, -1824, -1824, -1824, -1824,  1318,  1314,
     752,  1407, -1824, -1824,  1511,  1517,  1523,  1252,  1532, -1824,
    1536,  1270, -1824, -1824, -1824, -1824, -1824, -1824, -1824,  1537,
   -1824, -1824, -1824, -1824, -1824,  1266, -1824, -1824, -1824, -1824,
   -1824, -1824, -1824, -1824, -1824, -1824, -1824,  1542,   924, -1824,
   -1824, -1824, -1824,  1543, -1824, -1824, -1824, -1824, -1824, -1824,
   -1824, -1824, -1824, -1824, -1824, -1824, -1824, -1824, -1824, -1824,
   -1824, -1824,  1082, -1824, -1824, -1824, -1824, -1824, -1824, -1824,
   -1824, -1824, -1824, -1824, -1824, -1824, -1824,  1546, -1824,  1456,
   -1824, -1824,  1454, -1824, -1824, -1824, -1824, -1824, -1824, -1824,
   -1824, -1824,  1137, -1824,  1182,  1521, -1824, -1824, -1824, -1824,
   -1824, -1824, -1824,  1018,  1309,  1707,  1707, -1824, -1824, -1824,
    1417, -1824, -1824, -1824, -1824,  1419,  1422, -1824,  1423,  1426,
    1428,   623, -1824, -1824, -1824, -1824, -1824, -1824, -1824, -1824,
   -1824,  1567, -1824, -1824,  1568, -1824, -1824, -1824, -1824, -1824,
   -1824, -1824, -1824, -1824, -1824, -1824, -1824,  1271, -1824,  1286,
   -1824, -1824, -1824, -1824, -1824, -1824, -1824,  1573,  1582,  1325,
   -1824, -1824, -1824, -1824, -1824,  1583,   219, -1824, -1824, -1824,
    1330, -1824, -1824, -1824, -1824, -1824, -1824, -1824,  1332,  1334,
    1335,  1585, -1824, -1824,   737, -1824, -1824, -1824,  1588, -1824,
   -1824, -1824, -1824,  7265,  2570,  2606,  1664,  6177, -1824,   171,
     248,  1682,  3963,  1407,  1407,  1598,  7650,  7265,  7265,  7265,
    7265,  1656,  7265, -1824, -1824, -1824,  1688, -1824, -1824,   542,
    1078,   252, -1824,   829,  1126,   228,  1669, -1824,  7265, -1824,
   -1824,   929,  1440,  1151,   302, -1824,  1003,  1577,   929, -1824,
   -1824, -1824, -1824, -1824, -1824, -1824, -1824, -1824, -1824, -1824,
   -1824,  1570,   572, -1824, -1824, -1824, -1824, -1824, -1824, -1824,
   -1824, -1824,   190,  1404,  1409,  1703,   236,   916,   954, -1824,
    1046, -1824, -1824, -1824,  7265,  1719,  7265, -1824, -1824, -1824,
     688,   699, -1824,  7265, -1824, -1824,  1359, -1824, -1824,  7265,
    7265,  7265,  7265,  7265,  1624,  7265,  7265,   215,  7265,  1557,
    7265,  1644,  1722,  1646,  2977,  2977, -1824, -1824, -1824,  7265,
    1216,  7265,  1216, -1824,  1714,  1727, -1824,   928, -1824,  7650,
   -1824, -1824,  1303,  1565,  1560, -1824,   929, -1824, -1824, -1824,
   -1824, -1824, -1824, -1824, -1824, -1824, -1824, -1824, -1824, -1824,
   -1824, -1824, -1824, -1824,  1377,  8576,  8576,  8576,  8576,  8576,
    8576,  8576,  8576,  8765,  8576,  8576,   489, -1824,  1161, -1824,
   -1824, -1824, -1824, -1824,  1653, -1824,   347,   500, -1824,  2379,
    3665,  2500,  2980,   227, -1824, -1824, -1824, -1824, -1824, -1824,
    1394,  1396,  1398, -1824, -1824, -1824, -1824, -1824, -1824, -1824,
   -1824, -1824, -1824, -1824, -1824, -1824, -1824, -1824,  1756, -1824,
   -1824, -1824, -1824, -1824, -1824, -1824, -1824, -1824, -1824, -1824,
   -1824, -1824, -1824, -1824, -1824, -1824,  3963, -1824, -1824, -1824,
   -1824, -1824, -1824, -1824, -1824, -1824, -1824, -1824, -1824, -1824,
   -1824, -1824, -1824, -1824, -1824, -1824, -1824, -1824, -1824, -1824,
   -1824, -1824, -1824, -1824, -1824, -1824, -1824, -1824, -1824, -1824,
   -1824, -1824, -1824, -1824, -1824, -1824, -1824, -1824, -1824, -1824,
    1402,  1403, -1824, -1824, -1824, -1824, -1824, -1824,  1325,   781,
    1672, -1824, -1824, -1824, -1824, -1824,  1340, -1824, -1824, -1824,
   -1824, -1824,  1481,  1055, -1824,  1388,   432, -1824,   824,   236,
   -1824,  1165, -1824, -1824,  7265,  7265,  1791, -1824, -1824,  1698,
    1698, -1824,   248, -1824, -1824, -1824, -1824, -1824, -1824, -1824,
   -1824, -1824,  1446,  1682,  6998,   248, -1824, -1824, -1824, -1824,
    7265, -1824, -1824, -1824, -1824,   120, -1824,  7650,   120,  7265,
    1766, -1824,  8495, -1824,  1621, -1824,  1500, -1824, -1824, -1824,
   -1824, -1824, -1824,  1151, -1824,  1710,  1698, -1824,   583, -1824,
    6487, -1824,  5065, -1824, -1824, -1824, -1824,  1809, -1824,  1058,
    1799, -1824,  1717, -1824,  1718, -1824,  1058, -1824, -1824,  1515,
   -1824,  1373, -1824, -1824,  1373, -1824,  1754,  1373, -1824, -1824,
    7265, -1824, -1824, -1824, -1824, -1824,  1263,  1432,  1480, -1824,
   -1824, -1824, -1824, -1824, -1824, -1824,  1824,  7265,  1674,  7265,
   -1824, -1824, -1824, -1824,  1216, -1824,  1216,  1557, -1824, -1824,
     475,  6661,   189, -1824, -1824, -1824,  1587,  1823, -1824, -1824,
    1303,  1565,  1560, -1824,   162,  1587, -1824, -1824,  1003,  8576,
    8765, -1824,  1741,  1810, -1824, -1824, -1824, -1824, -1824,   309,
     309,   309,   309,   309,  1746,   627,   309,   309,   309,   309,
   -1824, -1824, -1824,   394, -1824,  1488,   145, -1824,  1753, -1824,
   -1824, -1824,   394,   394,   394,   394,   394,  8177, -1824,  1698,
    1698,  1491,  1400,  1757,   773,  1327, -1824, -1824, -1824,   394,
     394,   394,   512, -1824,  8177,  1698,  1698,  1496,   773,  1327,
   -1824, -1824, -1824,   394,   394,   512,  1752,  1499,  1764, -1824,
   -1824, -1824, -1824, -1824,  3534,  4013,  2846,  4584,  1158, -1824,
   -1824, -1824, -1824, -1824, -1824, -1824,  4380,  1651, -1824, -1824,
    1761, -1824, -1824, -1824,  1859, -1824, -1824, -1824, -1824, -1824,
   -1824, -1824, -1824, -1824,  1770, -1824, -1824, -1824, -1824, -1824,
   -1824, -1824,  2179, -1824,  1257,   807,   608,  1773, -1824, -1824,
   -1824, -1824, -1824,  1404,  1409, -1824, -1824, -1824, -1824, -1824,
   -1824, -1824, -1824, -1824, -1824,  1515, -1824, -1824, -1824, -1824,
   -1824, -1824, -1824,  7265, -1824, -1824, -1824, -1824, -1824, -1824,
    7650,  1502,   248, -1824, -1824, -1824, -1824, -1824, -1824, -1824,
   -1824, -1824, -1824, -1824, -1824,  7265,   153,   153,   929,  1577,
   -1824, -1824, -1824, -1824, -1824, -1824, -1824, -1824, -1824, -1824,
    1151, -1824, -1824, -1824,   394, -1824,   572, -1824, -1824, -1824,
   -1824, -1824, -1824, -1824,  1584,   336, -1824, -1824,  1774, -1824,
   -1824, -1824, -1824, -1824, -1824,  7265, -1824,  1796, -1824,  1557,
    1557,  7650, -1824,   714,  1882,   929, -1824,  1587,  1587,  1701,
    1789, -1824, -1824, -1824, -1824, -1824, -1824, -1824, -1824, -1824,
   -1824, -1824,  1887, -1824,   309,   309,   309, -1824, -1824, -1824,
   -1824, -1824,   387, -1824, -1824, -1824, -1824, -1824,  1793, -1824,
   -1824, -1824, -1824, -1824, -1824, -1824,  1895, -1824,   394,   394,
    -123, -1824, -1824, -1824, -1824, -1824, -1824, -1824, -1824, -1824,
   -1824, -1824,  1905, -1824, -1824, -1824, -1824, -1824,  1383, -1824,
   -1824, -1824, -1824, -1824, -1824,  1037,   394,   394,  -123,  1225,
    1383, -1824, -1824, -1824,  1768,   387,   394, -1824, -1824, -1824,
   -1824, -1824, -1824,  2296,  2505,  1540, -1824, -1824, -1824, -1824,
    1812, -1824,  1325, -1824, -1824, -1824,  1562,   723,  1266, -1824,
   -1824, -1824, -1824, -1824,  1698,  1814,   723,  1820,  1266, -1824,
   -1824, -1824, -1824, -1824,  1819,  1266, -1824, -1824, -1824, -1824,
   -1824, -1824, -1824, -1824, -1824, -1824, -1824, -1824, -1824, -1824,
    6998, -1824, -1824, -1824, -1824, -1824, -1824, -1824, -1824, -1824,
   -1824,   236, -1824, -1824, -1824, -1824, -1824,  7265,  1575,  7650,
   -1824,  1258,  6661, -1824, -1824,  1740,   394,  1564,  1574,  1581,
    1591,  1593,  1746, -1824, -1824, -1824,  1594,  1600,  1601,  1605,
     224,   394, -1824, -1824,  1889,  8177, -1824, -1824, -1824, -1824,
     773, -1824,  1327, -1824,  8001, -1824, -1824, -1824,   602, -1824,
     264,   394,   394, -1824, -1824, -1824, -1824, -1824,  1934, -1824,
    1609, -1824, -1824,   394,   394, -1824,   394,   394,   394,   394,
     394, -1824,  1813,   394, -1824,  1613, -1824, -1824, -1824, -1824,
   -1824,  1614,   929, -1824, -1824,  1701, -1824, -1824, -1824, -1824,
   -1824, -1824,  1616,  1625,  1630, -1824, -1824, -1824, -1824, -1824,
   -1824,   207, -1824, -1824, -1824,  1842, -1824, -1824, -1824, -1824,
    8177, -1824,  5450, -1824, -1824, -1824, -1824, -1824, -1824,   394,
    1944, -1824,   394,  1980, -1824,   394,   773,  1896, -1824, -1824,
   -1824,  1141, -1824, -1824, -1824, -1824, -1824, -1824, -1824, -1824,
    1761, -1824,  1894, -1824, -1824, -1824, -1824, -1824, -1824, -1824,
   -1824, -1824,   723, -1824, -1824, -1824, -1824,  1740,  1344,  4728,
    2904,  4845,  2327, -1824, -1824, -1824,  1406,  1638,  2454,  1262,
     186, -1824,  1493, -1824,  1895, -1824, -1824, -1824, -1824, -1824,
   -1824, -1824, -1824,  8177, -1824, -1824,  1084, -1824,  1899,  1903,
   -1824,  2005,   208, -1824,   394, -1824,   394,   394,   394,   394,
     394,  2640,  2580,  2149,   394,   394,   394,   394, -1824, -1824,
     178,  1648,  1768, -1824,  1980, -1824, -1824, -1824, -1824,  1452,
    1894,   394, -1824, -1824, -1824, -1824, -1824, -1824,   394,   394,
     394, -1824, -1824, -1824, -1824, -1824, -1824,   387, -1824, -1824,
   -1824, -1824, -1824, -1824, -1824, -1824, -1824, -1824, -1824, -1824,
   -1824, -1824, -1824, -1824, -1824, -1824, -1824, -1824, -1824
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int16 yydefact[] =
{
       2,     0,     1,     0,     4,     5,     0,     0,     0,     0,
     438,   438,   438,   438,   438,   438,   438,   438,   442,   445,
     438,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   230,   444,     9,    28,    29,     0,
     438,   438,   438,   438,    68,    67,     3,     0,    71,     0,
     439,     0,   463,     0,    66,     0,   430,   431,     0,     0,
       0,     0,   630,    87,    89,     0,     0,   290,     0,   313,
       0,   343,    72,   438,    73,   438,   438,   438,   438,   438,
     438,   438,     0,   438,   438,   438,   438,    74,   438,    75,
     438,   438,   438,   438,   438,   438,   438,   438,     0,   438,
     438,   438,   438,    76,   438,    77,   438,   469,   438,   469,
     438,   469,   469,   438,   438,   469,   438,   469,     0,   438,
     469,   469,     0,   438,   469,   469,   469,   469,   438,   438,
     438,   469,    35,   438,   469,   438,   438,   438,   438,   469,
     469,   438,    47,   438,   438,   438,   438,   469,     0,   438,
      80,   438,    81,     0,     0,     0,   782,   752,   432,   433,
     434,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    25,    25,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   964,   965,   966,   967,   968,   969,   970,   971,   972,
     973,   974,   975,   976,   977,   978,   979,   980,   981,   982,
     983,   985,     0,   987,   986,     0,     0,     0,     0,    34,
       0,     0,    85,   748,   747,   765,   766,   767,   245,   762,
     763,   756,   955,   757,   760,   764,   761,   758,   759,   753,
    1069,  1070,  1071,  1072,  1073,  1074,  1075,  1076,  1077,  1078,
    1079,  1080,  1081,  1082,    53,  1087,  1088,  1089,  1090,  1084,
    1085,  1086,   754,  1336,  1337,  1338,  1339,  1340,  1341,  1342,
    1343,   755,     0,   242,   243,     0,    33,   226,     0,    21,
     228,   438,   438,   438,   438,   438,   438,   438,   438,     0,
     438,   438,   438,   438,    16,   231,    39,   232,   443,   440,
     441,   438,   438,   438,    13,   876,   849,   851,    70,    69,
     446,   448,     0,     0,     0,   465,   464,   466,     0,   621,
       0,   738,   739,   740,     0,   946,   947,   948,   518,   519,
     951,     0,     0,     0,     0,   536,   541,   548,     0,   578,
     599,   611,   612,   687,   693,   714,     0,     0,   991,     0,
       7,    92,   471,   473,   484,   485,   486,   487,   488,   516,
     498,   474,    61,   268,   513,   494,   522,     0,    12,    13,
     534,   542,    14,    59,   546,   583,    36,   573,    44,    46,
     602,    40,     0,    54,    60,   619,    40,   686,    40,   692,
      18,    24,   504,    45,   712,   510,     0,   511,   496,   495,
     784,   787,   789,   791,   793,   794,   801,   803,     0,   802,
     745,   521,   955,   499,   505,   497,   753,   514,    62,     0,
       0,    65,   457,     0,     0,     0,    91,   451,     0,    95,
     306,   305,     0,   454,     0,     0,     0,   630,   112,   114,
     290,     0,   313,   343,   438,   438,   438,    13,   876,   849,
     851,     0,    60,     0,   136,   137,   138,     0,   130,   131,
     139,     0,   132,   133,   141,   142,     0,   134,   135,     0,
     143,     0,   145,   146,   853,   854,   852,   438,    13,    36,
      44,    51,     0,    60,   201,   470,   203,   174,   175,   176,
     177,   438,   172,   178,   470,   171,   173,   179,   198,   197,
     196,   190,   438,   469,   194,   193,   195,   853,   854,   855,
     438,     0,    13,   180,   182,   184,   188,    36,    44,    51,
     186,    78,   213,   438,   210,   171,   211,   209,   215,     0,
     216,    13,   205,   207,    44,    79,   217,   218,   219,   220,
     223,   438,   222,     0,  1096,  1093,  1094,    56,     0,   773,
     774,   775,   776,   777,   779,     0,   996,   998,     0,   997,
      52,     0,     0,  1334,  1335,    56,  1098,  1099,    55,    20,
      55,  1102,  1103,  1104,  1105,    30,     0,     0,  1108,  1109,
    1110,  1111,  1112,     9,  1130,  1131,  1125,  1120,  1121,  1122,
    1123,  1124,  1126,  1127,  1128,  1129,     0,    28,    55,  1145,
    1144,  1143,  1146,  1147,  1148,    31,    55,  1151,  1152,  1153,
      32,  1162,  1163,  1155,  1156,  1157,  1159,  1158,  1160,  1161,
      29,  1174,    55,  1170,  1167,  1166,  1171,  1169,  1168,  1172,
    1173,    31,  1177,  1180,  1176,  1178,  1179,     8,  1183,  1182,
      19,  1185,  1186,  1187,    11,  1191,  1192,  1189,  1190,    57,
    1197,  1194,  1195,  1196,    58,  1244,  1238,  1241,  1242,  1236,
    1237,  1239,  1240,  1243,  1245,     0,  1198,    55,  1278,  1279,
       0,    15,  1224,  1223,  1216,  1217,  1218,  1202,  1203,  1204,
    1205,  1206,  1207,  1208,  1209,  1210,  1211,    53,  1220,  1219,
    1222,  1221,  1213,  1214,  1215,  1231,  1233,  1232,     0,    25,
       0,  1228,  1227,  1226,  1225,  1332,  1329,  1330,     0,  1331,
      49,    55,    28,  1349,  1023,    29,  1348,  1351,  1021,  1022,
      34,     0,    48,    48,     0,    48,  1355,    48,  1358,  1357,
    1359,     0,    48,  1346,  1345,    27,  1367,  1364,  1362,  1363,
    1365,  1366,    23,  1370,  1369,    17,    55,  1373,  1376,  1372,
    1375,    38,    37,   959,   960,   961,    51,   962,    34,    37,
     957,   958,  1038,  1039,  1045,  1031,  1032,  1030,  1040,  1041,
    1061,  1034,  1043,  1036,  1037,  1042,  1033,  1035,  1028,  1029,
    1059,  1058,  1060,    51,     0,    12,  1046,  1002,  1001,     0,
     801,     0,     0,    48,    27,    23,    17,    38,  1377,  1006,
    1007,   984,  1005,     0,   746,  1083,   225,   244,    82,   227,
      83,    60,   154,   155,   132,   156,   157,     0,   158,   160,
     161,   438,    13,    36,    44,    51,    86,    84,   233,   234,
     236,   235,   238,   239,   237,   240,   873,   873,   873,    97,
       0,     0,   573,     0,   460,   461,   462,     0,     0,     0,
       0,     0,   953,   952,   949,   950,     0,     0,     0,    37,
      37,     0,   530,     0,     0,    12,     0,     0,   567,     0,
       0,     0,     0,     0,     0,     0,     6,     0,     0,   805,
       0,   472,   475,   515,     0,   532,     0,   531,   492,   489,
       0,     0,   535,   537,     0,   543,   493,   500,     0,     0,
       0,     0,     0,     0,   547,   549,   581,   582,   568,     0,
     490,   571,   572,     0,   579,   491,   501,     0,   598,   502,
       0,    47,    16,     0,     0,    20,    30,     9,    28,   914,
      29,     0,   919,   917,   918,    14,     0,    40,    40,   904,
     905,     0,   648,   651,   653,   655,   657,   658,   663,   668,
     666,   667,   669,   671,   610,   635,   636,   646,   906,   637,
     644,   638,   645,   641,   642,     0,   639,   640,     0,   670,
     643,   503,   512,     0,     0,   627,   626,   620,   622,   506,
       0,   705,   706,   707,   685,   690,   703,   507,     0,   691,
     696,   508,   509,   708,     0,   730,   731,   713,   715,   718,
     728,     0,   750,     0,   749,     0,     0,     0,     0,     0,
       0,     0,     0,   939,   940,   941,   942,   943,   944,   945,
      20,    30,     9,    28,    31,   931,    29,    31,     8,    19,
      11,    57,    58,    53,    15,    25,    49,    40,     0,   921,
     889,   922,   798,   799,   901,   888,   878,   877,   893,   895,
     897,   899,   900,   887,   923,   924,   890,     0,     0,     0,
       0,     7,     0,   843,   842,   900,     0,     0,   399,    60,
     252,   269,   293,   325,   344,   467,   111,     0,     0,     0,
       0,   118,     0,     0,   873,   873,   873,   120,     0,     0,
     573,     0,   129,   153,     0,     0,     0,     0,     0,   144,
       0,     0,   873,   148,   151,   149,   152,   169,   189,     0,
     204,   170,   192,   191,    12,   438,   181,   185,   183,   187,
     212,   214,   206,   208,   221,   224,  1095,     0,     0,    55,
     770,   771,    22,     0,   994,   783,    42,    42,  1333,  1100,
    1097,  1106,  1101,    20,    28,    20,    28,  1107,  1132,  1133,
    1134,  1135,    28,  1117,  1142,  1141,  1150,  1149,  1154,  1165,
    1164,  1175,  1181,  1184,  1188,  1193,    10,  1262,  1268,  1266,
    1257,  1258,  1261,  1263,  1252,  1253,  1254,  1255,  1256,  1264,
    1259,  1260,  1265,  1200,  1267,  1199,  1280,    15,  1276,  1212,
    1230,  1229,  1234,  1284,  1281,  1282,  1283,  1285,  1286,  1287,
    1288,  1289,  1290,  1291,  1292,  1293,  1294,  1295,  1296,  1297,
    1298,  1315,    50,  1327,  1350,  1017,  1018,  1024,    48,  1019,
    1347,     0,  1352,  1354,     0,  1356,  1344,  1361,  1368,  1374,
    1371,   956,   963,   954,  1044,  1047,  1048,     0,  1050,     0,
    1049,  1051,  1052,    12,    12,  1053,  1025,     0,     0,   999,
    1379,  1378,  1380,  1381,  1382,     0,     0,   768,   168,   159,
       0,   873,   163,   166,   164,   167,   229,   241,     0,     0,
       0,     0,   364,    13,   583,   389,    36,   369,     0,    44,
     394,   850,    51,     0,    28,    29,   613,     0,    60,     0,
     732,   734,     0,     0,     0,     0,     0,     0,     0,     0,
       0,  1053,     0,    13,    36,    44,     0,   727,    45,   722,
     721,     0,   726,   724,   725,     0,   699,   701,     0,   517,
     818,     7,     7,   820,   814,   817,   900,   839,     7,   804,
     468,   278,   533,   539,   540,   538,   435,   544,   545,   562,
      20,     0,     0,   560,   556,   551,   552,   553,   554,   557,
     555,   550,     0,   584,   587,     0,     0,     0,     0,    53,
       0,   677,   915,   916,     0,   672,     0,   907,   910,   911,
     908,   909,   920,     0,   913,   912,     0,   635,   644,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   679,
       0,     0,     0,     0,     0,     0,   624,   625,   623,     0,
       0,     0,   694,   717,   722,   721,   716,     0,    10,     0,
     785,   786,   788,   790,   792,   795,     7,   523,   525,   800,
     908,   930,   909,   932,   929,   928,   934,   926,   927,   925,
     935,   933,   936,   937,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   884,   883,   900,   989,
    1068,   845,   844,    63,     0,    64,     0,     0,   109,     0,
       0,     0,     0,     0,    60,   252,   269,   293,   325,   344,
       0,     0,     0,    13,    36,    44,    51,   458,   447,   449,
     269,   452,   455,   344,    12,   202,   199,    12,     0,   778,
     772,   769,    52,   780,   781,  1113,  1115,  1114,  1116,    55,
    1137,  1139,  1138,  1140,  1119,    28,     0,  1274,  1246,  1271,
    1248,  1275,  1251,  1272,  1273,  1249,  1269,  1270,  1247,  1250,
    1277,  1312,  1311,  1313,  1314,  1320,  1302,  1303,  1304,  1305,
    1317,  1306,  1307,  1308,  1309,  1310,  1318,  1319,  1321,  1322,
    1323,  1324,  1325,  1326,    55,  1301,  1300,  1316,    49,  1020,
       0,     0,    28,    28,    29,    29,  1026,  1027,   999,   999,
       0,    26,  1004,  1008,  1009,    34,     0,   344,    12,   379,
     384,   374,     0,     0,    98,     0,     0,   105,     0,     0,
     100,     0,   107,   615,     0,     0,   614,   481,   735,     0,
       0,   826,   733,   821,  1262,  1266,  1261,  1265,  1267,    53,
      10,    10,     0,   813,     0,   811,    37,    37,    12,    12,
       0,    12,   478,   479,   480,     0,   709,     0,     0,     0,
       0,   808,     0,   809,     0,   536,     0,   578,    12,    13,
      14,    36,    44,   819,   829,     0,     0,   838,   815,   827,
     807,   806,     0,   561,    28,   565,   566,    53,   564,     0,
     589,   591,     0,   569,     0,   570,     0,   575,   574,   576,
     600,     0,   604,   601,     0,   606,     0,     0,   608,   678,
       0,   682,   684,   647,   649,   650,   652,   654,   656,   664,
     665,   659,   662,   661,   660,   674,   673,     0,     0,     0,
    1062,  1063,  1064,  1065,   688,   704,   695,   697,   729,   751,
       0,     0,     0,   526,   938,   886,   880,     0,   891,   892,
     894,   896,   898,   885,   796,   879,   797,   902,   903,     0,
       0,   796,     0,     0,    60,   401,   400,   403,   402,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      88,   254,   253,     0,   248,     0,     0,    55,     0,    90,
     271,   270,     0,     0,     0,     0,     0,     0,   302,     0,
       0,     0,     0,     0,     0,     0,    93,   295,   294,     0,
       0,     0,     0,   476,     0,     0,     0,     0,     0,     0,
      94,   327,   326,     0,     0,     0,     0,     0,     0,    13,
      96,   346,   345,   128,     0,     0,     0,     0,     0,   379,
     384,   374,   121,   126,   122,   127,     0,     0,   150,   200,
       0,   995,  1136,  1118,     0,  1299,  1328,  1353,  1360,  1054,
    1055,  1056,  1057,    41,     0,    26,  1000,  1016,  1012,  1011,
    1010,    34,     0,   165,     0,     0,     0,     0,    13,   366,
     365,   368,   367,   584,   587,    36,   391,   390,   393,   392,
      44,   371,   370,   373,   372,   576,    51,   396,   395,   398,
     397,   616,   618,     0,   824,   825,   822,  1235,   993,   992,
       0,     0,   812,   990,   988,   482,   483,    12,  1066,   723,
     719,   720,    45,    45,   700,     0,     0,     0,     7,   840,
     841,   833,   831,   835,   832,   834,   830,   823,   836,   837,
     816,   828,   520,   279,     0,   559,     0,   558,   593,   594,
     595,   596,   597,   586,     0,     0,   588,   590,     0,   580,
      55,    55,    47,    55,   675,     0,   681,     0,   683,   689,
     698,     0,   736,     0,     0,     7,   524,   882,   881,   631,
       0,   110,   459,   363,   450,   268,   453,   290,   313,   456,
     343,   251,   247,   249,     0,     0,     0,   363,   363,   363,
     363,   255,     0,   428,   429,    43,   427,   426,     0,   424,
     272,   274,   273,   277,   275,   288,   291,   287,     0,     0,
       0,   342,   341,    43,   340,   404,   406,   407,   405,   360,
     408,   361,    55,   359,   309,   310,   312,   311,     0,   308,
     296,   303,   304,   300,   477,     0,     0,     0,     0,     0,
       0,   338,   337,   335,     0,     0,     0,   349,   113,   115,
     116,   117,   119,     0,     0,     0,   140,   147,  1091,    10,
       0,  1003,   999,  1015,  1013,   162,     0,     0,     0,    12,
     381,   380,   383,   382,     0,     0,     0,     0,     0,    12,
     386,   385,   388,   387,     0,     0,    12,   376,   375,   378,
     377,   846,    99,   874,   875,   106,   101,   848,   108,   617,
       0,   743,  1067,   710,   711,   702,   810,   280,   563,   585,
     592,     0,    31,    31,   609,   607,   676,     0,     0,     0,
     737,     0,   528,   527,   632,   633,     0,     0,     0,     0,
       0,     0,     0,   363,   363,   363,     0,     0,     0,     0,
       0,     0,   352,   425,     0,     0,   298,   299,   301,   339,
       0,   292,     0,   297,     0,   328,   329,   336,   324,   334,
       0,     0,     0,   350,    12,    12,    12,  1092,     0,    26,
       0,    57,    53,     0,     0,   103,     0,     0,     0,     0,
       0,   104,     0,     0,   102,     0,   577,   603,   605,   680,
     741,     0,     7,   634,   628,   631,   399,   269,   293,   325,
     344,   250,     0,     0,     0,   364,   389,   369,   394,   357,
     356,     0,   353,   358,   276,     0,   289,   362,   307,    60,
       0,   286,     0,    13,    36,    44,    51,   285,   284,     0,
       0,   321,     0,   281,   323,     0,     0,     0,   418,   412,
     411,   415,   410,   413,   414,   347,   348,   124,   125,   123,
       0,  1014,     0,   859,   858,   865,   867,   870,   871,   868,
     869,   872,     0,   861,   744,   742,   529,   633,     0,     0,
       0,     0,     0,   379,   384,   374,     0,     0,     0,     0,
       7,   351,   423,   320,   283,   314,   315,    12,   316,   318,
     317,   319,   331,     0,   333,    55,     0,   419,     0,     0,
    1201,     0,     0,   862,     0,   629,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   355,   354,
       0,     0,     0,   322,   282,   332,    55,   417,   416,     0,
       0,     0,    55,    60,   256,   257,   258,   259,     0,     0,
       0,    13,    36,    44,    51,   420,   421,     0,   409,   330,
     436,   437,   864,   863,    43,   860,   267,    12,    12,    12,
     260,   265,   261,   266,   422,   866,   263,   264,   262
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
   -1824, -1824,    -1, -1227,   985,    70, -1326,   984,  -335,  -313,
    -863,  -734,  1094,  1222, -1824,   990,  -539, -1824, -1824,  1232,
   -1824,  -150, -1577,  1235,   -26,   -37,  1459,  -600, -1824, -1824,
    -702, -1824,  -421,  -727,  1223, -1824,  -289, -1824,   894, -1811,
    -430, -1195, -1824,  -900,  -482,  -924, -1824,  -512,   546,  -661,
   -1824,  -517,  1476, -1014,  1011, -1824,  -449, -1824,    12, -1824,
   -1824,  2024, -1824, -1824, -1824,  1763,  1765,   164,  1785, -1824,
   -1824, -1824,  1912,  1836, -1824, -1824, -1824, -1824, -1824, -1824,
   -1824, -1824, -1824, -1824, -1824, -1824,     8, -1824,  1611, -1824,
   -1824,   -52,   586,  -353, -1395, -1824, -1824, -1824, -1767, -1636,
    -424, -1824, -1418,  -406,   280,   -70,  -419, -1824,   -71, -1824,
   -1824, -1424, -1421, -1824,  -429, -1414, -1823, -1824,  -192,   -60,
   -1627,  -949,  -114,  -113, -1682, -1677, -1674,  -111,  -115,   -98,
   -1824, -1824,  -219, -1824, -1824, -1824, -1824, -1824, -1824, -1824,
   -1824,   139,  -760, -1538, -1824,   176,   -47,  3358, -1824,   392,
   -1824, -1824,  2297, -1824,   342,   182,  1792, -1824,   993, -1824,
    -514,  1647, -1824, -1824,   255,   305,   525,   142,   -41, -1824,
   -1824, -1423, -1824, -1824, -1824, -1824, -1824, -1824, -1351,  -344,
   -1824, -1824, -1824, -1824, -1824, -1824, -1824,   149, -1824, -1233,
   -1824, -1220, -1824, -1824,  1187, -1824, -1216, -1824, -1824, -1824,
   -1824,  1181, -1824, -1824, -1824,   180, -1824, -1207, -1824,  1711,
   -1465,   232, -1824, -1198, -1824,   815,   249,   515,   253,   516,
     440,   443, -1824, -1824, -1050, -1824, -1824, -1824, -1824, -1824,
   -1824, -1824,  1119,  -362,  1665,   -57,  -132,  -580,   728,   730,
     731, -1824,  -813, -1824, -1824, -1824, -1824, -1824, -1824,  1733,
   -1824, -1824, -1824, -1824, -1824, -1824, -1824, -1824, -1824,  -385,
     711, -1824, -1824, -1824, -1824,  1118,   502,  -919,   501,  1249,
     717, -1343, -1824, -1824,  1802, -1824, -1824, -1824,   -63,  1513,
    -842,  -368,  1620, -1633,  2093, -1824, -1824, -1824,   997,    -8,
   -1824, -1824, -1824, -1824, -1824,  -193,  -208, -1824, -1824,   681,
    -828,  1911,   -53, -1824,   800,   263, -1824, -1545, -1824, -1824,
     529, -1824, -1373, -1824,   499, -1313,   503, -1824, -1824,  1720,
    -605,  1691,  -582,  1690,  -579,  1694,   168, -1824, -1752, -1824,
   -1824,  -149, -1824, -1824,  -607,  -545,  1709, -1824,  -389,  -317,
    -858,  -864,  -850, -1824,  -372,  -857, -1824,   -82,  -138,  -870,
   -1824, -1505,  -323,    99,  1822, -1824,   -28, -1824,   101, -1824,
   -1401, -1824,   102, -1824, -1824, -1824, -1824, -1824,   298,  -280,
     326,  1437,   379,  1826,  1827, -1824, -1824,  -532,   203, -1824,
   -1824, -1824,   593,   -58, -1824, -1824,   -50, -1824,   -40, -1824,
      -3, -1824,   -59, -1824, -1824, -1824,   -35, -1824,   -17, -1824,
      -9, -1824,    -4, -1824,     6, -1824,     7, -1824,    16, -1824,
      21, -1824,    26, -1824,    27,  1466, -1824,   -72, -1824, -1824,
   -1824, -1824, -1824, -1824, -1824, -1824, -1824, -1824,  1501, -1019,
   -1824, -1824, -1824, -1824, -1824,    33, -1824, -1824, -1824, -1824,
     969, -1824, -1824,    37, -1824,    43, -1824, -1824, -1824, -1824,
   -1824, -1824, -1824, -1824, -1824, -1824, -1824, -1824, -1824, -1824,
   -1824, -1824, -1824, -1824, -1824, -1824
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,     1,  1791,   877,  1161,  1369,  1506,  1163,  1246,   839,
     896,  1188,   826,  1228,   991,  1162,  1367,   810,  1491,  1227,
     992,   698,  1827,  1226,  1420,  1422,  1368,  1155,  1157,   808,
     801,   521,   910,  1231,  1230,   827,   919,  2031,  1493,  2113,
     915,   993,   916,   535,  1219,  1213,  1548,  1106,  1134,   805,
     971,  1139,  1126,  1164,  1165,   897,   972,   883,  1792,    46,
      47,    48,    74,    87,    89,   456,   460,   465,   452,   103,
     294,   105,   492,   493,   132,  1110,   484,   142,   150,   152,
     276,   279,   296,   297,   835,  1266,   277,   222,   424,  1743,
    1952,  1953,  1459,   425,  1460,  1642,  2198,  2199,  1975,  1976,
     428,  1769,  1461,   429,  1998,  1999,   432,  2257,  2201,  2202,
    2206,  1462,  1770,  1983,   434,  1463,  2111,  2181,  2182,  1991,
    1992,  2097,  1573,  1578,  1836,  1834,  1835,  1576,  1581,  1457,
    1993,  1752,  2131,  2211,  2212,  2213,  2292,  1753,  1754,  1965,
    1966,  1942,   223,  1338,  2322,    49,    50,    61,   459,    52,
     463,  1945,   467,   468,  1947,    71,   473,  1950,   454,   455,
    1943,   315,   316,   317,    53,   436,  1591,   496,  1756,   352,
     353,  1772,   354,   355,   356,   357,   358,   359,   360,   361,
     362,   363,   364,   365,   366,  1417,  1702,  1703,   367,   368,
     887,   369,   370,   892,   893,   371,   372,   895,   373,   374,
     904,   905,  1350,  1344,  1647,  1648,   375,   376,   908,  1278,
    1659,  1919,   377,   378,   914,   909,  1653,  1353,  1655,  1354,
    1355,  1913,   379,   380,  1662,   918,   381,   382,   383,   384,
     385,   977,   978,  1728,   423,  2095,  2164,   942,   943,   944,
     945,   946,   947,   948,  1682,   949,   950,   951,   952,   953,
     954,   386,   387,   984,   388,   389,   989,   390,   391,   985,
     986,   392,   393,   394,   997,   998,  1306,  1307,  1308,   999,
    1000,  1289,  1290,   395,   396,   397,   398,   399,   224,   955,
    1003,  1040,   956,   227,   400,   229,  1132,   554,   555,   957,
     562,   401,   402,   403,   404,   405,   406,  1042,  1043,  1044,
     407,   408,   409,   878,   879,  1604,  1605,  1322,  1323,  1324,
    1592,  1898,  1593,  1638,  1633,  1634,  1639,  1325,  1888,  1062,
    1842,   840,  1854,   842,  1860,   843,   477,   511,  2143,  2060,
    2272,  2273,  2043,  2053,  1268,  1849,   841,   410,  1063,  1064,
    1048,  1049,  1050,  1051,  1326,  1053,   958,   959,   960,  1056,
    1057,   411,   854,   961,   759,   760,   232,   413,   962,   560,
    1561,   789,   963,  1256,   802,  1565,  1831,   235,   964,   721,
     966,   722,   967,   784,   785,  1243,  1244,   786,   968,   969,
     414,   415,   970,  2029,   240,   548,   241,   569,   242,   575,
     243,   583,   244,   597,  1152,  1505,   245,   605,   246,   610,
     247,   620,   248,   631,   249,   637,   250,   640,   251,   644,
     252,   649,   253,   654,   254,   688,   689,   690,   255,   691,
     256,   704,   257,   699,   258,   417,   666,  1183,  1598,  1185,
    1518,  1510,  1515,  1508,  1512,   259,   671,  1211,  1547,  1530,
    1536,  1525,  1212,   260,   710,   261,   565,   262,   263,   735,
     264,   723,   265,   725,   266,   727,   267,   732,   268,   742,
     269,   745,   270,   751,   271,   798
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      44,   300,    54,  1093,   990,   430,   855,  1119,   790,   882,
     884,   299,   351,    45,  1083,   431,  1080,  1427,  1217,  1046,
    1327,  1362,   230,  1082,   426,   412,  1189,   803,  1004,   700,
    1140,  1160,  1233,   888,  1108,  1081,  1052,   412,   848,  1785,
     280,  1783,  1312,  1101,  1797,   230,   309,  1065,  1065,  1796,
    1105,  1864,  1865,  1141,   418,  1798,   889,   230,  1104,   421,
     485,  1069,   485,  1099,   485,   494,  1700,   874,   494,  1807,
     485,  1795,  1372,   485,   494,  1403,  1107,   494,   494,   494,
     494,  1154,  1699,  1281,   494,  1806,  1079,   494,  1118,  1156,
    1628,  1047,   494,   494,  1621,  1623,  1117,   979,  1899,   987,
     485,  1640,   965,  1629,  1123,  1159,   298,  1630,   703,  1097,
    1773,  1773,  1432,  1616,  1855,  1319,  1631,  1890,   677,  2025,
    1330,   677,  2023,  1121,  1977,  1632,  2024,   674,  1376,   231,
     674,   233,   234,  1233,  1087,  1410,  1411,   675,  2005,  1345,
     675,  1977,   678,  1349,  1413,   678,    58,    59,    60,  1412,
    1186,  2009,   231,  1832,   233,   234,  1934,  1823,  1825,  1414,
     679,   876,   761,   679,   231,  1103,   233,   234,   680,  1418,
    -752,   680,  2119,   681,   676,  1001,   681,   676,  -752,  -752,
    -752,  -752,  1328,   682,   683,  1067,   682,   683,   310,  1701,
     311,   876,  2132,   684,  1214,  1001,   684,  1935,   685,  1116,
     422,   685,   312,   686,   687,   427,   686,   687,   230,   433,
     692,  1649,   230,   692,   693,  2250,  2300,   693,  1122,  1866,
     694,  1978,  1979,   694,   157,  1963,  1964,   980,     3,  1229,
    1269,  1270,     4,  1295,     5,  1273,  1407,  2006,  2007,  1001,
     885,  1220,   350,  1222,  1232,  1223,  1136,   221,  2033,   486,
    1225,   488,  1245,   490,     6,     7,     8,     9,    56,   501,
    1279,  1010,   505,   443,  1282,   853,   853,   457,    65,  1786,
    1055,  1234,  1787,  1599,  1868,  1869,   527,  1933,  1336,   472,
     806,  1055,  1055,   158,   159,   160,   809,  2207,  -847,   540,
     510,  2179,  2180,    55,  2148,  1650,  1276,    73,   500,   528,
    1430,   530,   556,   557,   558,   761,   606,  1012,  1665,   272,
    1668,  1250,  1274,  1265,  1617,   231,   695,   233,   234,   231,
    1896,   233,   234,   696,   870,   412,  1054,   607,   236,  2179,
    2180,  1281,  1301,  1002,  1386,  1094,   856,  1054,  1054,  1589,
     608,  1657,  1590,   451,   638,   230,   712,    88,  2186,  1773,
    1095,   236,     4,   697,     5,  1045,   237,   867,  -847,  1129,
     804,   639,  1258,   236,   104,  1275,  1045,  1045,  2208,  2209,
     715,   151,  1429,   857,  1785,   868,  1783,   278,   430,   237,
    1387,  1563,    56,   295,   158,   159,   160,  1722,   431,  1096,
    1260,   237,  1788,  1635,  1264,  1076,  1636,    56,   464,     4,
    1628,     5,  1263,   871,    64,  1628,   430,  1435,   412,   238,
    1092,  1259,  1137,  1629,    56,  1143,   431,  1630,  1629,   430,
    1436,  2004,  1630,   412,  1421,   301,  1631,  1423,   230,   431,
    1144,  1631,   238,     3,  2004,  1632,    56,     4,   522,     5,
    1632,  2080,   419,   230,   238,  1773,  1773,  1588,   420,   816,
     886,    57,   231,  1520,   233,   234,   485,   821,   457,     6,
       7,     8,     9,   988,  -439,   435,   430,   852,   852,   439,
     819,   858,  1337,   981,   982,   983,   431,  1470,  1471,  1472,
    2274,  1931,  1977,  1473,   462,   435,  1651,  1814,  2200,  -246,
     221,  2203,   313,   314,  2334,  1484,  2186,  1435,   609,  1866,
    1296,     3,   489,  1130,  1131,     4,  2146,     5,  1475,  1262,
    1719,  1068,  1476,  2335,   236,   221,  1416,     4,   236,     5,
    1789,   221,  1452,   221,   559,   435,   526,     6,     7,     8,
       9,  -752,  1658,  1600,  1601,   231,   537,   233,   234,  1067,
    1329,   221,   237,   543,  1474,  1683,   237,  1934,   221,  1191,
     231,  -747,   233,   234,  2254,  1936,   221,  1977,  -747,  -747,
    -747,  -747,  2221,  2283,  1618,  -719,  2281,  1708,  1709,  1389,
    2282,  1153,  1680,  2251,  2301,    57,  1711,  1271,  1564,  2266,
     350,  1710,  2004,  1158,   507,   221,  2162,  1896,  2088,   815,
      57,  1712,   422,  1790,   221,   238,   310,  -747,   880,   238,
     311,   273,   274,   275,  1495,   427,  1497,    57,   444,     3,
     433,  1920,  1490,     4,  1921,     5,  2156,  1923,   645,   302,
    1458,   303,   646,   239,  1816,  1098,  2210,  -268,  1192,    57,
    2294,  2139,  1651,   541,   221,     6,     7,     8,     9,   677,
    2189,   677,  1291,  1004,  1280,  1113,   416,  1272,  1374,  1375,
     674,   236,   880,  1147,  1568,   647,   648,  -439,   416,   724,
     675,  2076,  1311,   678,   311,   678,  1434,  1628,  1446,   273,
     274,   275,    56,   561,  1635,   814,   435,  1636,  1216,   237,
    1629,   679,  1125,   679,  1630,  1448,  1215,  2073,  2074,   680,
    2204,   680,   474,  1631,   681,  1419,   681,   676,  1666,  1883,
    1100,   847,  1632,  2137,   682,   683,   682,   683,  2092,   981,
     982,   983,   911,   912,   684,  1464,   684,  1645,  1954,   685,
    2089,   685,  1646,  2054,   686,   687,   686,   687,   714,   502,
     503,   692,   238,   692,   236,   693,  1549,   693,  1544,  1055,
    1447,   694,   712,   694,  1274,  2241,  2161,    56,  1433,   236,
    2240,   726,  2110,  1624,   430,  1545,  2242,   221,  1390,   733,
    1391,  2321,   237,  1277,   431,  1889,   715,  1893,  1625,   743,
    1582,   792,  2239,   336, -1017,   873, -1017,   237,   508,  1486,
     476,  1145,   880,  1327,   734, -1018,  1626, -1018,  1546,  1312,
    1624,  1627,   973,   974,   744,  1054,  1146,   509,  1845,  1674,
    1675,  1643,   445,  1378,   446,  1625,  1681,  1309,     3,   239,
     336,   807,     4,   239,     5,   238,   712,  1415,  1785,   844,
    1783,  1281,   787,  1626,  1321,     3,   563,   564,  1627,     4,
     238,     5,   788,   845,     6,     7,     8,     9,  -760,  1587,
     715,  1932,   230,  1267,  1327,  -760,  -760,  -760,  -760,  1580,
     880,     6,     7,     8,     9,  1577,   849,   718,   719,  1339,
     846,  1624,   230,  1828,   851,    57,  1724,  1055,  1055,  1055,
    1055,  1055,   799,   800,  1055,  1614,  1625,   881,  1985,  1873,
    1874,   336,   667,  1613,  -760,  1431,   475,  2055,   476,  1773,
    1773,  2141,  1340,  1371,  1626,  2193,  2142,   866,  2044,  1627,
    1055,  1010,  1370,  1986,  1987,  2045,   869,  1341,  1556,  1557,
     875,  -747,  1955,  2194,  1956,  1120,  1392,  1011,  2195,  1055,
    2196,  1004,   872,  1054,  1054,  1054,  1054,  1054,  -573,   880,
    1054,  1404, -1017,   880,   876,  2236,   668,   669,  1867,   720,
      57,   975,  2046, -1018,   976,   670,   416,  1012,  1002,   231,
     894,   233,   234,  1342,   890,   891,  1054,  1357,   880,  1045,
    1574,   898,   133,   134,  1805,   135,   136,   137,  1007,   231,
    1358,   233,  1313,   641,  2056,  1054,  1566,  2047,  1393,   917,
     650,    90,  1812,  1008,   642,   643,  1907,  1359,  1009,    91,
    1612,    92,  1624,    93, -1017,  1058,    94,    95,    96,  1384,
      97,  1385,   230,    62,  1045, -1018,   705,  1625,  2106,  2107,
    2108,  2109,   336,   651,   652,  1793,   653,  1697,  1059,  1004,
     157,  1660,  2084,  2288,  1454,  1626,  1444,  1815,  1988,   416,
    1627,  1661,  1070,   304,   305,   306,   307,   549,   550,   706,
     707,  1071,  1343,  2124,   416,  1804,  2115,  1706,  1989,  1990,
     308,   906,   907,  1803,  1715,     4,     3,     5,   157,  1660,
       4,  1453,     5,  1705,  1707,  1455,   911,   912,   437,  1664,
    1713,  1707,  1716,  1718,  1072,   708,   447,   448,   449,   450,
    2090,  1073,     6,     7,     8,     9,  2048,  -766,  1603,     4,
     712,     5,  1074,  2120,  -766,  -766,  -766,  -766,   312,   483,
    1442,  -720,   899,   900,   901,   902,   903,  -847,   709,   231,
     714,   233,   234,  1443,   715,  1444,   512,  1109,  1496,  -343,
    1498,   517,   518,   519,  1127,  1560,  1504,  2223,  1824,  2034,
    1128,   531,   746,  -766,   534,  -761,   728,  -856,   911,   912,
    1408,  1360,  -761,  -761,  -761,  -761,   747,  -857,   236,  1808,
     157,  1660,  1809,   748,  2172,  2173,  2174,   880,  1115,     3,
    1802,  1667,  1442,     4,  2268,     5,     3,  2269,  1314,  1442,
       4,   749,     5,  2049,   880,  1454,   237,  1444,   750,   551,
     552,  -761,  1720,   553,  1444,     6,     7,     8,     9,  1133,
    1850,  1135,     6,     7,     8,     9,   237,  1499,  -760,  1187,
    1786,  1291,  1895,  1787,   570,   138,   571,   572,   573,   574,
    1894,  1221,   139,   140,  1500,  1247,   729,   730,   731,  1501,
    1502,    98,  1957,  1958,  1959,  1960,  1002,  1224,   141,   238,
    1968,  2128,  1248,  1833,  2120,  1327,    99,  1005,  1006,   718,
     719,  1378,  1378,  1378,  1378,  1378,  1521,  1378,  1378,   238,
    1522,  1523,  1524,  1249,   100,  1255,  1691,  1691,     3,   101,
    1871,   102,     4,     3,     5,   995,   996,     4,   793,     5,
     158,   159,   160,  1875,  1876,  1941,  1878,   794,   795,  1379,
    1380,   796,   797,  1257,     6,     7,     8,     9,   230,     6,
       7,     8,     9,  1891,   836,   837,   838,  1055,  1055,  1055,
    1055,  1055,  1055,  1055,  1055,  1055,  1055,  1055,   236,  1929,
     544,  1930,  1287,   545,   546,   547,  1892,   321,   322,  1437,
    1438,   720,   323,  1788,  1002,  1292,  1503,    18,    19,  1293,
    1937,  1938,  1513,  1514,  1684,  1294,   237,  1297,   325,   326,
     327,  1886,  1887,   330,  2068,     3,  1298,  1707,  1707,     4,
    1271,     5,  2036,  1054,  1054,  1054,  1054,  1054,  1054,  1054,
    1054,  1054,  1054,  1054,   566,   567,   568,   158,   159,   160,
    1299,     6,     7,     8,     9,  1516,  1517,    35,  1908,  1909,
    1910,  1911,  1912,  1067,  1552,  1553,   736,    36,     4,   238,
       5,  2122,  2037,    37,   632,   231,  1300,   233,   234,  1554,
    1555,   230,   737,  2082,  2083,   880,  2085,     3,  1302,  1311,
     633,     4,  1318,     5,  1994,  1995,  1996,    38,  1997,   634,
    2066,  1838,  1331,   738,  2065,  1332,   635,   636,  1333,  1624,
     739,  1366,   412,     6,     7,     8,     9,  1084,  1085,  1086,
    2103,  2104,  2105,   239,  1625,   876,  1622,  -766,  1373,   336,
    1381,  1789,   230,  1829,  1830,  1723,  1725,  1334,  1741,  1750,
    1767,  1781,  1626,   239,   273,   274,   275,  1627,  1346,  1726,
    1102,  1742,  1751,  1768,  1782,  2121,  2017,   157,  1660,  1813,
    1347,  2224,  2157,  2158,  1055,  1148,  1149,  1150,  1151,  1281,
    1348,   143,  1352,   144,  1356,  -761,  1981,  1982,   145,  1382,
     146,  1383,  1055,  1114,   147,   994,   995,   996,   231,  1388,
     233,   234,   158,   159,   160,   880, -1062,  1821,  1822,   740,
     741,  2101, -1063,  2099,  2022,  2062,  1819,  1820,  1394,  2100,
    2315,  1856,  1526,  1527,  1528,  1529,  2038,  2290,  2291,  1395,
    1054,     3,  2072,   225,   148,     4,   149,     5,   981,   982,
     983,  1396,  1880,  1397,  1309,  1404,  2320,  1336,  1054,   231,
    1399,   233,   234,  1055,   855,  1400,   225,     6,     7,     8,
       9,  1407,  1839,  1409,  2118,  1846,   412,  1851,   225,  1440,
    1857,  1055,  1055,  1439,  1441,  1840,  1445,  1321,  1847,  1451,
    1852,   655,  2098,  1858,   236,  1435,   230,  1692,  1693,  1456,
     656,  1904,  2127,   239,  1465,  1067,  1466,   880,  1467,   230,
    1468,  1469,   880,  1271,   412,  1477,   657,  1480,  1905,  1054,
     658,  1478,   237,  2039,   659,   660,  1274,  1479,  2287,   661,
     662,   663,   664,   665,   230,  1483,  1481,  1054,  1054,     3,
    1482,  1903,  1485,     4,  1507,     5,  1488,  1492,  1509,  1538,
     226,  1511,     3,  1519,  1539,  2054,     4,  1540,     5,  1541,
    1235,  1236,  1603,  1542,  1543,     6,     7,     8,     9,  1560,
    1584,  1550,  1551,   226,  1237,   238,   711,  1558,     6,     7,
       8,     9,  1238,  1967,  2261,   226,  1559,  1562,   712,  1572,
     713,  1010,  1579,  1786,   880,  1567,  1787,  1569,  1239,  1570,
    1571,  1271,  1602,   231,  2145,   233,   234,   236,   714,  1610,
    2276,  1615,   715,  1619,  2151,   716,   231,  1652,   233,  1313,
    1644,  2154,  1654,  1291,  1656,  1670,  2155,  1673,  1679,   225,
    1687,  1688,  1689,   225,   828,   237,   829,  -719,   830,   831,
    2253,   231,  1961,   233,   234,  1704,   832,   833,  2295,   230,
    -720,  1970,  1971,  1972,  1973,  1974,   880,  1721,   236,  1799,
     598,  1800,   599,  1801,  1810,  2260,   230,   412,  2000,  2001,
    2002,  2003,  2284,  2259,  1817,  1818,  1826,  1837,   412,  2319,
     834,   600,  2011,  2012,  2013,  2325,   237,   230,   238,   601,
     602,   603,   604,  1741,  1750,  1767,  1781,  1863,   230,  2217,
    2218,  2219,  2333,  1336,   717,  1750,  1742,  1751,  1768,  1782,
    1885,  1870,   868,   862,  1261,  1897,  1788,  1906,  1751,  2055,
    -592,  1914,  1915,   853,   853,  1922,  1240,  1241,  1242,   770,
    1925,  1442,  1918,  2040,  2050,  2057,   226,   718,   719,   238,
     226,  1927,   780,   781,   782,  1939,  2041,  2051,  2058,  1637,
    1940,  1951,  1962,  1969,  2326,  1980,   231,  2014,   233,   234,
    2008,  1984,   230,  2015,  2016,  2028,   225,  2030,  2071,  1944,
    1946,  1946,  1949,   231,  2032,   233,   234,  2061,  2081,  2079,
    2258,  1291,  2087,  2332,   478,   479,   480,   481,  2091,   239,
     880,  2331,  2094,  2096,   231,  2102,   233,   234,  2114,  2112,
     576,   577,   236,  2077,  2115,   231,  2136,   233,   234,   578,
     579,   580,   581,   582,  2120,  1314,  2138,   430,  2147,   720,
    2130,  1039,  2293,   230,  2149,  2152,  2140,   431,  2163,  2166,
     237,  2185,  1039,  1039,  1531,  1532,  1533,  1534,  1535,  2167,
     236,  2160,  2220,   237,  1789,   430,  2168,  2252,  2232,   225,
    1274,   499,  2112,  2263,  1055,   431,  2169,   506,  2170,  2175,
     513,   514,   515,   516,   225,  2176,  2177,   520,   237,   231,
    2178,   233,   234,   226,  2222,   532,   533,  2116,  2117,  2234,
    2235,  2243,  1727,   238,  1744,  1755,  1771,  1784,   880,  -287,
    2244,  2144,  2336,  2337,  2338,  2245,   238,  2123,  2330,  2271,
    2267,  2150,   239,  2297,  2285,  2125,  2126,  2298,  2153,  2129,
    1054,  2299,  2317,  1424,  1426,  2133,  1363,  2027,  1253,  1425,
    1254,   238,  2040,  2050,  2057,   852,   852,  1252,  1041,  1251,
     231,  1494,   233,   234,  1142,  2041,  2051,  2058,  1811,  1041,
    1041,  1138,   412,  1428,    72,   812,   524,  2183,   813,  1078,
    2171,  1794,  2188,   416,   491,   236,   226,  2205,  2289,  2010,
    2187,  2246,   230,  2249,  2248,  2247,   811,  2214,  2238,    18,
      19,   226,   236,  2318,  1948,   822,   823,   824,   825,  1335,
     820,   230,  1075,   237,  2093,  1351,  2078,  2067,   913,  1575,
    1843,  1844,  2063,   236,  1916,  2165,  1398,  2064,  1841,  1917,
     237,  1848,  1077,  1853,   236,  2275,  1859,   230,  2237,  1676,
    2184,  1695,  1677,  -438,  1678,  1406,   230,  1879,  1882,    35,
    1315,   237,   850,   228,  1698,  1489,  1717,   791,  1641,    36,
    2215,  2216,   237,  2070,  1872,    37,   238,  1900,  1088,  1090,
    1066,  1901,  2225,  2226,  1091,  2227,  2228,  2229,  2230,  2231,
       3,  2323,  2233,   238,     4,   860,     5,  1089,  1218,    38,
     864,   865,  2270,  1637,  1190,  1537,  1184,     0,   236,   231,
       0,   233,   234,     0,   238,     0,     6,     7,     8,     9,
       3,     0,   230,     0,     4,   238,     5,  2183,   231,     0,
     233,   234,     0,     0,     0,     0,   237,   416,  2262,     0,
       0,  2264,     0,     0,  2265,     0,     6,     7,     8,     9,
     239,   412,     0,     0,   231,     0,   233,   234,     0,     0,
       0,  1786,     0,   231,  1787,   233,   234,  2316,     0,   236,
       0,   230,     0,     0,     0,   416,     0,  1725,  1750,  1767,
    1781,     0,     0,     0,     0,  1839,  1846,  1851,  1857,   238,
    1726,  1751,  1768,  1782,  2112,   230,     0,   237,  1840,  1847,
    1852,  1858,     0,     0,  2054,  2296,     0,     0,     0,     0,
       0,     0,     0,  2302,     0,  2303,  2304,  2305,  2306,  2307,
    2040,  2050,  2057,  2311,  2312,  2313,  2314,     0,     0,   231,
       0,   233,   234,  2041,  2051,  2058,     0,     3,     0,     0,
    2324,     4,     0,     5,     0,     0,  -438,  2327,  2328,  2329,
     238,    67,    68,    69,   491,     0,   523,     0,     0,  1744,
    1755,  1771,  1784,     6,     7,     8,     9,     0,     3,    18,
      19,  1755,     4,     0,     5,     0,     0,     0,   231,     0,
     233,   234,     0,     0,  1788,     0,     0,     0,     0,     0,
     239,     0,     0,     0,     6,     7,     8,     9,     0,  2042,
    2052,  2059,   231,   225,   233,   234,     0,   239,   236,  1786,
       0,     0,  1787,  -438,     0,   440,   441,   442,     0,    35,
       3,     0,     0,   225,     4,     0,     5,   236,   416,    36,
    1039,  2036,   469,   470,     0,    37,   237,     0,     0,   416,
       0,   482,     0,     0,     0,     0,     6,     7,     8,     9,
     497,   498,     0,   236,     0,   237,   504,  1729,     0,    38,
       0,     0,   236,     0,     0,     0,     0,  1730,  2055,     0,
       0,  2037,  1731,   529,  1732,     0,  1733,     0,     0,     0,
       0,   237,   538,   539,     0,     0,   542,     0,     0,   238,
     237,     0,     0,     0,  1377,     3,     0,     0,     0,     4,
       0,     5,     0,   239,     0,     0,     0,     0,   238,     0,
     226,     0,  1789,     0,     0,     0,     0,     0,     0,     0,
       0,     6,     7,     8,     9,     0,     0,     0,   236,     0,
    1310,     0,  1788,     0,   238,     0,     0,  1041,     0,   880,
       0,     3,     0,   238,     0,     4,     3,     5,     0,     0,
       4,     0,     5,     0,     0,  2310,   237,     0,  1039,  1039,
    1039,  1039,  1039,   225,   239,  1039,     0,     6,     7,     8,
       9,     0,     6,     7,     8,     9,     0,   236,     0,     0,
       0,  1757,     0,     0,  1734,  2035,     0,     0,  2042,  2052,
    2059,  1039,     0,     0,     0,     0,     0,     0,  -573,     0,
       0,   236,     0,     0,     0,   237,  -438,     0,     0,   238,
    1039,     0,     0,     0,     0,  2038,     0,     0,     0,     0,
    1286,     3,  1288,   817,   818,     4,     0,     5,     0,   237,
    1758,  1759,  1760,  1761,     0,  1762,  2044,     0,  1763,  1303,
    1304,     0,  1305,  2045,     0,  1316,  1317,     6,     7,     8,
       9,     0,     0,     0,  1405,  1764,     0,     0,   238,     0,
    1789,     0,     0,     0,     0,  1041,  1041,  1041,  1041,  1041,
     226,     0,  1041,     0,     0,     0,     0,     0,     0,     0,
    2046,     3,   238,     0,     0,     4,   880,     5,     0,     0,
       0,     0,     0,  1361,     0,     0,  1365,  1735,  1041,     0,
       0,     0,  2134,   416,     0,  1765,     0,     6,     7,     8,
       9,  2044,  1736,     0,     0,  2047,     0,  1041,  2045,     0,
       0,     0,   239,   584,   585,     0,     0,   586,     0,     0,
    1737,     0,     0,  2280,     0,  1738,     0,  1739,   587,   588,
     589,   590,   591,   592,   593,   594,   595,     0,   239,   621,
     622,     0,   623,     0,     0,  2046,     0,   239,     0,   611,
     612,  1402,     0,     0,     0,     0,     0,     0,     0,   880,
       0,   624,     0,     0,   613,  2036,   614,   615,   616,   625,
     626,     0,   627,     0,     0,  1740,     0,     0,   596,     0,
    2047,     0,   628,   629,   630,     0,     0,     0,     0,   617,
     618,   619,     0,  1727,  1755,  1771,  1784,     0,   911,   912,
       0,  1841,  1848,  1853,  1859,  2037,     0,     0,  1624,     0,
       0,     0,     0,   239,  2048,     0,     0,     0,     0,     0,
       0,  1449,  1450,  1625,     0,     0,     0,     0,   336,  1112,
       0,     0,     0,     0,   880,     0,  2042,  2052,  2059,   225,
       0,  1626,     0,     0,     0,     0,  1627,     0,     0,     0,
    2286,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   416,     0,     0,     0,     0,     0,  1124,     0,
       0,     0,     0,     0,     0,     0,     0,     3,     0,     0,
     880,     4,     0,     5,     0,   880,   239,     0,     0,  2048,
       0,     0,     0,     0,     0,     0,  1766,     0,     0,     0,
       0,  2135,     0,     6,     7,     8,     9,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,  1757,     0,     0,
       0,     0,  1377,  1377,  1377,  1377,  1377,     0,  1377,  1377,
       0,     0,     0,     0,     0,     3,     0,  1690,  1690,     4,
      75,     5,     0,     0,     0,     0,   226,     0,    76,  2038,
      77,     0,   225,     0,     0,    78,    79,    80,     0,    81,
     880,     6,     7,     8,     9,     0,  1758,  1759,  1760,  1761,
       0,  1762,     0,     0,  1763,  1757,  2309,     0,  1039,  1039,
    1039,  1039,  1039,  1039,  1039,  1039,  1039,  1039,  1039,     0,
       0,  1764,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   225,     0,     0,     0,  1663,  1663,     0,
    1663,     3,     0,     0,     0,     4,     0,     5,     0,     0,
     880,     0,     0,     0,  1758,  1759,  1760,  1761,     0,  1762,
       0,     0,  1763,     0,     0,     0,  2308,     6,     7,     8,
       9,  1765,  1166,     0,     0,     0,  1583,     0,     0,  1764,
    1586,  1774,     0,     0,     0,     0,     0,  1167,     0,   226,
    1606,  1607,  1608,  1609,     0,  1611,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   921,     0,
       0,  1620,     0,     0,   922,  1041,  1041,  1041,  1041,  1041,
    1041,  1041,  1041,  1041,  1041,  1041,     0,     0,     0,  1765,
       0,  1775,  1776,  1777,     0,  1762,     0,     0,  1763,     0,
     226,     0,   158,   159,   160,     0,     0,     0,     0,  1020,
       0,     0,  1168,  1021,     0,  1778,     0,  1669,  1022,  1671,
       0,     0,     0,     0,  1023,     0,  1672,     0,   929,     0,
       0,     0,     0,     0,     0,     0,     0,   225,     0,     0,
       0,  1685,     0,  1686,  1624,     0,     0,     0,  1026,     0,
     225,     0,  1694,     0,  1696,  1039,     0,     0,     0,  1625,
       0,     0,     0,     0,   336,  1779,     0,     0,     0,     0,
      82,     0,     0,  1039,     0,   225,     0,  1626,     0,     0,
       0,     0,  1627,     0,  1169,    83,     0,  1170,  1171,  1172,
    1173,  1174,  1175,  1176,  1177,  1178,  1179,  1180,  1181,  1182,
       0,     0,  1624,    84,     0,     0,     0,     0,    85,     0,
      86,     0,     0,     0,     0,     0,   880,  1625,     0,     0,
       0,     0,   336,     0,     0,     0,     0,     0,     0,     0,
       0,     0,  2020,     0,  1039,  1626,     0,     0,     0,     0,
    1627,     0,     0,     0,   226,     0,     0,     0,     0,     0,
       0,     0,  1039,  1039,     0,  1881,     0,  1310,  1405,     0,
       0,     0,  1041,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   880,     0,     0,     0,  1624,     0,
    1041,     0,   226,     0,     0,   932,     0,     0,   933,   934,
    2278,     0,     0,  1625,     0,   935,     0,     0,   336,     0,
       0,  1663,     0,     0,  1663,     0,     0,  1663,     0,     0,
       0,  1626,     0,     0,     0,     0,  1627,     0,     0,     0,
       0,     0,     0,     0,     0,   937,   938,     0,   225,     0,
       0,     0,    10,     0,     0,     0,     0,  1861,  1862,   225,
      11,  1041,    12,     0,    13,     0,     0,    14,    15,    16,
     880,    17,     0,     0,     0,    18,    19,     0,     0,  1041,
    1041,     0,     0,  1877,     0,     0,  1780,   221,     0,     0,
       0,     0,  1884,     0,     0,     0,     0,     0,     0,    51,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    63,
      51,    51,    66,    66,    66,    70,     0,     0,    51,     0,
       0,     0,     0,   225,     0,    35,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    36,     0,     0,     0,     0,
       0,    37,     0,  1924,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   226,     0,     0,     0,     0,
    1926,     0,  1928,     0,     0,    38,   226,     0,     0,     0,
       0,    51,     0,     0,   438,    51,    66,    66,    66,    70,
       0,     0,     0,     0,   225,     0,    51,     0,   453,   458,
     461,    51,   466,    66,    66,   471,     0,   453,   453,   453,
     453,     0,    66,     0,     0,     0,   487,     0,    51,     0,
     495,    66,    66,   495,    70,     0,     0,    66,     0,   495,
       0,     0,   495,   495,   495,   495,     0,     0,     0,   495,
     226,    51,   525,   466,    66,   466,     0,   495,   495,     0,
       0,   536,    51,    66,    66,     0,     0,    66,     0,    51,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   281,     0,     0,     0,     0,     0,     0,     0,   282,
       0,   283,     0,   284,     0,     3,   285,   286,   287,     4,
     288,     5,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   226,    39,     0,     0,     0,     0,     0,     0,     0,
       0,     6,     7,     8,     9,     0,     0,    40,     0,     0,
       0,     0,  1729,     0,     0,     0,     0,     0,     0,     0,
       0,     0,  1730,   225,     0,    41,     0,  1731,     0,  1732,
      42,  1733,    43,     0,     0,     0,  2069,     0,     0,     0,
       0,     0,   225,     0,     0,  1039,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   106,     0,     0,  2075,     0,
     107,     0,     0,   108,   109,   110,   111,     0,     0,   112,
     113,     0,   114,   115,   116,     0,   117,     0,     0,   453,
     458,   461,    51,   466,    66,    66,   471,     0,   453,   453,
     453,   453,     0,     0,     0,     0,     0,     0,  2086,     0,
       0,     0,     0,     0,     0,     0,     3,     0,     0,     0,
       4,     0,     5,     0,     0,     0,   118,     0,   119,   120,
     121,     0,     0,     0,     0,     0,     0,     0,     0,   153,
     226,     0,     6,     7,     8,     9,   154,   155,     0,  1734,
       0,   318,   156,   319,     0,     0,     0,     0,     0,   226,
    1745,     0,  1041,     0,     0,     0,     0,     0,   320,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   321,   322,
       0,     0,     0,   323,     0,     0,   324,     0,     0,     0,
       0,     0,     0,     0,     0,   325,   326,   327,   328,   329,
     330,     0,   225,     0,     0,     0,     0,     0,     0,     0,
    1746,   289,     0,     0,   331,     0,   332,     0,     0,   157,
     158,   159,   160,     0,     0,   161,   290,   162,     0,     0,
    1747,   163,     0,     0,     0,     0,   164,     0,     0,     0,
       0,     0,   165,     0,   291,     0,     0,     0,     0,   292,
       0,   293,     0,     0,     0,     0,     0,     0,   166,     0,
       0,     0,  1735,   167,     0,     0,   168,     0,     0,     0,
    2159,   169,     0,     0,     0,   170,     0,  1736,   171,   172,
    1748,     0,     0,   173,     0,     0,   174,     0,   175,     0,
       0,     0,     0,     0,     0,  1737,     0,     0,     0,  1111,
    1738,     0,  1739,     0,     0,   122,     0,     0,     0,   226,
      66,     0,     0,   176,   177,     0,   178,   179,   180,   181,
     123,     0,     0,     0,     0,     0,     0,   124,   125,   126,
     127,    51,     0,     0,   880,     0,     0,     0,   128,     0,
       0,     0,     0,   129,     0,   130,   131,     0,     0,    66,
    2018,     0,     0,     0,   182,   183,   184,   185,     0,     0,
       0,     0,     0,   186,   187,     0,     0,   188,   189,   333,
     191,   192,   193,   194,   195,   196,   197,   198,   199,   200,
     201,   202,   203,   204,   205,   206,   207,   208,   209,   210,
     211,     0,     0,   334,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   335,     0,
       0,     0,     0,   336,     0,     0,     0,     0,     0,     0,
     337,     0,     0,     0,     0,     0,   338,     0,     0,     0,
       0,   339,     0,     0,     0,     0,     0,   213,   214,     0,
     340,     0,     0,   341,   342,   343,   344,     0,     0,     0,
     345,     0,  1594,     0,   346,   347,   215,     0,     0,     0,
     216,   217,     0,     0,     3,   880,     0,     0,     4,   348,
       5,     0,   218,   219,     0,     0,     0,     0,   349,     0,
     350,  1749,   220,     0,     0,   221,     0,   153,     0,     0,
       6,     7,     8,     9,   154,   155,     0,     0,     0,   318,
     156,   319,     0,     0,     0,     0,     0,     0,  1745,     0,
       0,     0,     0,     0,     0,     0,   320,  1168,     0,     0,
       0,     0,     0,     0,     0,     0,   321,   322,     0,     0,
     752,   323,     0,     0,   324,     0,     0,     0,     0,     0,
       0,     0,     0,   325,   326,   327,   328,   329,   330,     0,
       0,     0,     0,     0,     0,     0,     0,   753,  1746,     0,
     754,     0,   331,     0,   332,     0,     0,   157,   158,   159,
     160,     0,     0,   161,     0,   162,     0,   755,  1747,   163,
       0,     0,     0,     0,   164,     0,     0,     0,     0,  1595,
     165,     0,  1170,  1171,  1596,  1173,  1174,  1175,  1176,  1177,
    1178,  1179,  1180,  1181,  1597,     0,   166,     0,     0,     0,
       0,   167,     0,     0,   168,  1193,     0,     0,     0,   169,
       0,     0,     0,   170,     0,     0,   171,   172,  1748,     0,
       0,   173,     0,     0,   174,     0,   175,     0,  1194,  1195,
    1196,  1197,  1198,  1199,  1200,  1201,  1202,  1203,  1204,  1205,
    1206,  1207,  1208,  1209,  1210,     0,     0,     0,     0,     0,
       0,   176,   177,     0,   178,   179,   180,   181,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     191,   192,   193,   194,   195,   196,   197,   198,   199,   200,
     201,   202,   203,   204,   205,   206,   207,   208,   209,   210,
     211,     0,   182,   183,   184,   185,     0,     0,     0,     0,
       0,   186,   187,     0,     0,   188,   189,   333,   191,   192,
     193,   194,   195,   196,   197,   198,   199,   200,   201,   202,
     203,   204,   205,   206,   207,   208,   209,   210,   211,     0,
       0,   334,     0,   756,     0,     0,     0,   213,   214,     0,
       0,     0,     0,     0,     0,     0,   335,   757,     0,     0,
       0,   336,     0,     0,     0,     0,     0,     0,   337,     0,
       0,     0,     0,     0,   338,     0,     0,     0,     0,   339,
       0,     0,     0,   758,     0,   213,   214,     0,   340,     0,
       0,   341,   342,   343,   344,     0,     0,     0,   345,     0,
       0,     0,   346,   347,   215,     0,     0,     0,   216,   217,
       0,     0,     0,   880,     0,     0,     0,   348,     0,   861,
     218,   219,     0,     0,     0,     0,   349,     0,   350,  2019,
     220,     3,     0,   221,     0,     4,     0,     5,     0,     0,
       0,   762,   763,     0,     0,   764,     0,     0,     0,     0,
       0,     0,     0,     0,   153,     0,     0,     6,     7,     8,
       9,   154,   155,   765,     0,     0,   318,   156,   319,     0,
       0,     0,     0,     0,     0,  1745,     0,     0,     0,     0,
       0,     0,     0,   320,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   321,   322,     0,     0,     0,   323,     0,
       0,   324,     0,     0,     0,     0,     0,     0,     0,     0,
     325,   326,   327,   328,   329,   330,     0,     0,     0,     0,
     862,     0,     0,  1487,     0,  1746,     0,     0,     0,   331,
       0,   332,     0,     0,   157,   158,   159,   160,     0,     0,
     161,     0,   162,     0,     0,  1747,   163,     0,     0,     0,
       0,   164,     0,     0,     0,     0,     0,   165,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   166,     0,     0,     0,     0,   167,     0,
       0,   168,     0,   766,     0,     0,   169,     0,     0,     0,
     170,     0,     0,   171,   172,  1748,     0,     0,   173,     0,
       0,   174,     0,   175,     0,     0,     0,   767,   768,   769,
     770,   771,   772,   863,   773,   774,   775,   776,   777,   778,
     779,     0,     0,   780,   781,   782,     0,     0,   176,   177,
       0,   178,   179,   180,   181,     3,     0,     0,     0,     4,
       0,     5,     0,     0,     0,     0,   783,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     6,     7,     8,     9,     0,     0,     0,     0,   182,
     183,   184,   185,     0,     0,  1774,     0,     0,   186,   187,
       0,     0,   188,   189,   333,   191,   192,   193,   194,   195,
     196,   197,   198,   199,   200,   201,   202,   203,   204,   205,
     206,   207,   208,   209,   210,   211,     0,     0,   334,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   335,     0,  1775,  1776,  1777,   336,  1762,
       0,     0,  1763,     0,     0,   337,     0,     0,     0,     0,
       0,   338,     0,     0,     0,     0,   339,     0,     0,  1778,
       0,     0,   213,   214,     0,   340,     0,     0,   341,   342,
     343,   344,     0,     0,     0,   345,     0,     0,     0,   346,
     347,   215,     0,     0,     0,   216,   217,     0,     0,     3,
     880,     0,     0,     4,   348,     5,     0,   218,   219,     0,
       0,     0,     0,   349,     0,   350,  2026,   220,     0,  1779,
     221,     0,   153,     0,     0,     6,     7,     8,     9,   154,
     155,     0,     0,     0,   318,   156,   319,     0,     0,     0,
       0,     0,     0,  1745,     0,     0,     0,     0,     0,     0,
       0,   320,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   321,   322,     0,     0,     0,   323,     0,     0,   324,
       0,     0,     0,     0,     0,     0,     0,     0,   325,   326,
     327,   328,   329,   330,     0,     0,     0,     0,     0,     0,
       0,     0,     0,  1746,     0,     0,     0,   331,     0,   332,
       0,     0,   157,   158,   159,   160,     0,     0,   161,     0,
     162,     0,     0,  1747,   163,     0,     3,     0,     0,   164,
       4,     0,     5,     0,     0,   165,     0,     0,     0,     0,
       0,     0,  1624,     0,     0,     0,     0,     0,     0,     0,
       0,   166,     6,     7,     8,     9,   167,  1625,     0,   168,
       0,     0,   336,     0,   169,     0,  1774,     0,   170,     0,
       0,   171,   172,  1748,     0,  1626,   173,     0,     0,   174,
    1627,   175,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   176,   177,     0,   178,
     179,   180,   181,     0,   880,     0,  1775,  1776,  1777,     0,
    1762,     0,     0,  1763,     0,     0,     0,     0,     0,     0,
    2021,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    1778,     0,     0,     0,     0,     0,     0,   182,   183,   184,
     185,     0,     0,     0,     0,     0,   186,   187,     0,     0,
     188,   189,   333,   191,   192,   193,   194,   195,   196,   197,
     198,   199,   200,   201,   202,   203,   204,   205,   206,   207,
     208,   209,   210,   211,     0,     0,   334,     0,     0,     0,
    1779,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   335,     0,     0,     0,     0,   336,     0,     0,     0,
       0,     0,     0,   337,     0,     0,     0,     0,     0,   338,
       0,     0,     0,     0,   339,     0,     0,     0,     0,     0,
     213,   214,     0,   340,     0,     0,   341,   342,   343,   344,
       0,     0,     0,   345,     0,     0,     0,   346,   347,   215,
       4,     0,     5,   216,   217,     0,     0,     0,   880,     0,
       0,     0,   348,     0,     0,   218,   219,     0,     0,   153,
       0,   349,     0,   350,  2277,   220,   154,   155,   221,     0,
       0,   318,   156,   319,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   320,     0,
       0,     0,     0,  1624,     0,     0,     0,     0,   321,   322,
       0,     0,   859,   323,     0,     0,   324,     0,  1625,     0,
       0,     0,     0,   336,     0,   325,   326,   327,   328,   329,
     330,     0,     0,     0,     0,     0,  1626,     0,     0,   753,
       0,  1627,   754,     0,   331,     0,   332,     0,     0,   157,
     158,   159,   160,     0,     0,   161,     0,   162,     0,   755,
       0,   163,     0,     0,     0,     0,   164,     0,     0,     0,
       0,     0,   165,     0,     0,   880,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   166,     0,
       0,  2279,     0,   167,     0,     0,   168,     0,     0,     0,
       0,   169,     0,     0,     0,   170,     0,     0,   171,   172,
       0,     0,     0,   173,     0,     0,   174,     0,   175,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   176,   177,     0,   178,   179,   180,   181,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   191,   192,   193,   194,   195,   196,   197,   198,
     199,   200,   201,   202,   203,   204,   205,   206,   207,   208,
     209,   210,   211,     0,   182,   183,   184,   185,     0,     0,
       0,     0,     0,   186,   187,     0,     0,   188,   189,   333,
     191,   192,   193,   194,   195,   196,   197,   198,   199,   200,
     201,   202,   203,   204,   205,   206,   207,   208,   209,   210,
     211,     0,     0,   334,     0,   756,     0,   920,     0,   213,
     214,     0,   762,   763,     0,     0,   764,     0,   335,   757,
       0,     0,     0,   336,     0,   153,     0,     0,     0,     0,
     337,     0,     0,   155,   765,     0,   338,     0,   156,     0,
       0,   339,     0,     0,     0,   758,     0,   213,   214,     0,
     340,     0,     0,   341,   342,   343,   344,     0,     0,     0,
     345,     0,     0,     0,   346,   347,   215,     0,     0,     0,
     216,   217,   921,     0,     0,     0,     0,     0,   922,   348,
       0,     0,   218,   219,     0,     0,     0,   923,   349,   924,
     350,  1902,   220,     0,     0,   221,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   157,   158,   159,   160,     0,
       0,   161,     0,   925,     0,     0,     0,   926,     0,     0,
       0,     0,   927,     0,     0,     0,     0,     0,   928,     0,
       0,     0,   929,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   166,     0,     0,     0,     0,   167,
       0,     0,   930,     0,   766,     0,     0,   169,     0,     0,
       0,   170,     0,     0,   171,   172,     0,     0,     0,   173,
       0,     0,   174,     0,   175,     0,     0,     0,   767,   768,
     769,   770,   771,   772,     0,   773,   774,   775,   776,   777,
     778,   779,     0,     0,   780,   781,   782,     0,     0,   176,
     177,  2255,   178,   179,   180,   181,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,  1283,   783,     0,     0,
       0,     0,     0,   762,   763,     0,     0,   764,     0,     0,
       0,     0,     0,     0,   153,     0,     0,     0,     0,     0,
     931,     0,   155,     0,     0,   765,     0,   156,     0,     0,
       0,     0,     0,     0,     0,   190,   191,   192,   193,   194,
     195,   196,   197,   198,   199,   200,   201,   202,   203,   204,
     205,   206,   207,   208,   209,   210,   211,     0,     0,   212,
       0,   921,     0,     0,     0,     0,     0,   922,     0,   932,
       0,     0,   933,   934,     0,     0,     0,     0,     0,   935,
       0,     0,     0,     0,     0,     0,     0,   936,     0,     0,
       0,     0,     0,     0,   157,   158,   159,   160,     0,     0,
     161,     0,   925,   213,   214,     0,   926,     0,     0,   937,
     938,   927,     0,     0,     0,     0,     0,  1284,     0,     0,
       0,   929,   215,     0,     0,     0,   216,   217,     0,     0,
       0,     0,     0,   166,     0,     0,   939,   940,   167,   758,
       0,  1285,     0,     0,     0,   766,   169,     0,   941,     0,
     170,   221,     0,   171,   172,     0,     0,     0,   173,     0,
       0,   174,     0,   175,     0,     0,     0,     0,     0,   767,
     768,   769,   770,   771,   772,  2256,   773,   774,   775,   776,
     777,   778,   779,     0,     0,   780,   781,   782,   176,   177,
       0,   178,   179,   180,   181,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,  1364,     0,     0,   783,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   153,     0,     0,     0,     0,     0,   931,
       0,   155,     0,     0,     0,     0,   156,     0,     0,     0,
       0,     0,     0,     0,   190,   191,   192,   193,   194,   195,
     196,   197,   198,   199,   200,   201,   202,   203,   204,   205,
     206,   207,   208,   209,   210,   211,     0,     0,   212,     0,
     921,     0,     0,     0,     0,     0,   922,     0,   932,     0,
       0,   933,   934,     0,     0,     0,     0,     0,   935,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   157,   158,   159,   160,     0,     0,   161,
       0,   925,   213,   214,     0,   926,     0,     0,   937,   938,
     927,     0,     0,   161,     0,   162,  1284,     0,     0,   163,
     929,   215,     0,     0,   164,   216,   217,     0,     0,     0,
     165,     0,   166,     0,     0,   939,   940,   167,   758,     0,
    1285,     0,     0,     0,     0,   169,   166,   941,     0,   170,
     221,   167,   171,   172,   168,     0,     0,   173,     0,   169,
     174,     0,   175,   170,     0,     0,   171,   172,   672,     0,
       0,   173,     0,     0,   174,     0,   175,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   176,   177,     0,
     178,   179,   180,   181,     0,     0,     0,     0,     0,     0,
       0,   176,     0,   673,  1401,     0,     0,   181,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   153,     0,     0,     0,     0,     0,   931,     0,
     155,     0,     0,     0,     0,   156,     0,     0,     0,     0,
       0,     0,     0,   190,   191,   192,   193,   194,   195,   196,
     197,   198,   199,   200,   201,   202,   203,   204,   205,   206,
     207,   208,   209,   210,   211,     0,     0,   212,     0,   921,
       0,     0,     0,     0,     0,   922,     0,   932,     0,     0,
     933,   934,     0,     0,     0,     0,     0,   935,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   157,   158,   159,   160,     0,     0,   161,     0,
     925,   213,   214,     0,   926,     0,     0,   937,   938,   927,
       0,     0,   161,     0,   162,  1284,     0,   701,   163,   929,
     215,     0,     0,   164,   216,   217,     0,     0,     0,   165,
       0,   166,     0,     0,   939,   940,   167,   758,     0,  1285,
       0,     0,     0,     0,   169,   166,   941,     0,   170,   221,
     167,   171,   172,   168,     0,     0,   173,     0,   169,   174,
       0,   175,   170,     0,     0,   171,   172,     0,     0,     0,
     173,     0,     0,   174,     0,   175,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   176,   177,     0,   178,
     179,   180,   181,     0,     0,     0,     0,     0,     0,     0,
     176,     0,   702,  1585,     0,     0,   181,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   153,     0,     0,     0,     0,     0,   931,     0,   155,
       0,     0,     0,     0,   156,     0,     0,     0,     0,     0,
       0,     0,   190,   191,   192,   193,   194,   195,   196,   197,
     198,   199,   200,   201,   202,   203,   204,   205,   206,   207,
     208,   209,   210,   211,     0,     0,   212,     0,   921,     0,
       0,     0,     0,     0,   922,     0,   932,     0,     0,   933,
     934,     0,     0,     0,     0,     0,   935,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   157,   158,   159,   160,     0,     0,   161,     0,   925,
     213,   214,     0,   926,     0,     0,   937,   938,   927,     0,
       0,   161,     0,   162,  1284,     0,     0,   163,   929,   215,
       0,     0,   164,   216,   217,     0,     0,     0,   165,     0,
     166,     0,     0,   939,   940,   167,   758,     0,  1285,     0,
       0,     0,     0,   169,   166,   941,     0,   170,   221,   167,
     171,   172,   168,     0,     0,   173,     0,   169,   174,     0,
     175,   170,     0,     0,   171,   172,     0,     0,     0,   173,
       0,     0,   174,     0,   175,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   176,   177,     0,   178,   179,
     180,   181,     0,     0,     0,     0,     0,     0,     0,   176,
       0,     0,     0,     0,     0,   181,  1013,  1014,  1015,  1016,
    1017,  1018,     0,     0,     0,     0,  1019,     0,     0,     0,
    1001,     0,     0,     0,     0,     0,   931,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   190,   191,   192,   193,   194,   195,   196,   197,   198,
     199,   200,   201,   202,   203,   204,   205,   206,   207,   208,
     209,   210,   211,     0,     0,   212,     0,   921,     0,     0,
       0,     0,     0,   922,     0,   932,     0,     0,   933,   934,
       0,     0,     0,     0,     0,   935,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     157,   158,   159,   160,     0,     0,     0,     0,  1020,   213,
     214,     0,  1021,     0,     0,   937,   938,  1022,     0,  1320,
       0,     0,     0,  1023,     0,     0,     0,   929,   215,     0,
       0,     0,   216,   217,     0,     0,     0,     0,     0,  1024,
       0,     0,   939,   940,  1025,   758,     0,  1026,     0,     0,
       0,     0,  1027,     0,   941,     0,  1028,   221,     0,  1029,
    1030,     0,     0,     0,  1031,     0,     0,  1032,   921,  1033,
       0,     0,     0,     0,   922,     0,     0,     0,     0,     0,
       0,   161,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,  1034,     0,     0,  1035,   165,     0,
    1036,   157,   158,   159,   160,     0,     0,     0,     0,  1020,
       0,     0,     0,  1021,   166,     0,     0,     0,  1022,   167,
       0,     0,   168,     0,  1023,     0,     0,   169,   929,     0,
       0,   170,     0,     0,   171,   172,     0,     0,     0,   173,
    1024,     0,   174,     0,   175,  1025,     0,     0,  1026,     0,
       0,     0,     0,  1027,     0,     0,     0,  1028,     0,     0,
    1029,  1030,     0,     0,     0,  1031,     0,     0,  1032,   176,
    1033,     0,     0,     0,     0,   181,     0,     0,     0,     0,
       0,     0,     0,     0,   932,     0,     0,   933,   934,     0,
       0,     0,     0,  1320,   935,  1034,     0,     0,  1035,     0,
       0,  1036,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   937,   938,     0,  1037,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   921,     0,     0,     0,     0,     0,   922,     0,
       0,   939,   940,     0,     0,     0,     0,     0,     0,     0,
       0,   350,     0,  1038,     0,     0,   221,     0,     0,     0,
       0,     0,     0,     0,     0,   157,   158,   159,   160,     0,
       0,     0,     0,  1020,     0,   932,     0,  1021,   933,   934,
       0,     0,  1022,     0,     0,   935,     0,     0,  1023,     0,
    1060,  1001,   929,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,  1024,     0,     0,     0,     0,  1025,
       0,     0,  1026,     0,     0,   937,   938,  1027,  1037,     0,
       0,  1028,     0,     0,  1029,  1030,     0,     0,     0,  1031,
       0,     0,  1032,     0,  1033,     0,     0,     0,   921,     0,
       0,     0,   939,   940,   922,     0,     0,     0,     0,     0,
       0,     0,   350,     0,  1038,     0,     0,   221,     0,  1034,
       0,     0,  1035,     0,     0,  1036,     0,     0,     0,     0,
       0,   157,   158,   159,   160,     0,     0,     0,     0,  1020,
       0,     0,     0,  1021,     0,     0,     0,     0,  1022,     0,
       0,     0,     0,     0,  1023,     0,     0,     0,   929,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    1024,     0,     0,     0,     0,  1025,     0,     0,  1026,     0,
       0,     0,     0,  1027,     0,     0,     0,  1028,     0,     0,
    1029,  1030,     0,     0,     0,  1031,     0,     0,  1032,     0,
    1033,     0,     0,     0,     0,     0,     0,     0,     0,   932,
       0,     0,   933,   934,     0,     0,     0,     0,     0,   935,
       0,     0,     0,     0,     0,  1034,     0,     0,  1035,     0,
       0,  1036,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   937,
     938,     0,  1037,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   939,   940,     0,     0,
       0,     0,   153,     0,     0,     0,     0,     0,  1038,   154,
     155,   221,     0,     0,   318,   156,   319,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   320,     0,     0,     0,   932,     0,     0,   933,   934,
       0,   321,   322,     0,     0,   935,   323,     0,     0,   324,
       0,     0,     0,     0,     0,     0,     0,     0,   325,   326,
     327,   328,   329,   330,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   937,   938,   331,  1037,   332,
       0,     0,   157,   158,   159,   160,     0,     0,   161,     0,
     162,     0,     0,     0,   163,     0,     0,     0,     0,   164,
       0,     0,   939,   940,     0,   165,     0,     0,     0,     0,
       0,     0,  1061,     0,  1038,     0,     0,   221,     0,     0,
       0,   166,     0,     0,     0,     0,   167,     0,     0,   168,
       0,     0,     0,     0,   169,     0,     0,     0,   170,     0,
       0,   171,   172,     0,     0,     0,   173,     0,     0,   174,
       0,   175,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   176,   177,     0,   178,
     179,   180,   181,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   182,   183,   184,
     185,     0,     0,     0,     0,     0,   186,   187,     0,     0,
     188,   189,   333,   191,   192,   193,   194,   195,   196,   197,
     198,   199,   200,   201,   202,   203,   204,   205,   206,   207,
     208,   209,   210,   211,     0,     0,   334,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   153,
       0,   335,     0,     0,     0,     0,   336,   155,     0,     0,
       0,     0,   156,   337,     0,     0,     0,     0,     0,   338,
       0,     0,     0,     0,   339,     0,     0,     0,     0,     0,
     213,   214,     0,   340,     0,     0,   341,   342,   343,   344,
       0,     0,     0,   345,     0,     0,   921,   346,   347,   215,
       0,     0,   922,   216,   217,     0,     0,     0,     0,     0,
       0,     0,   348,     0,     0,   218,   219,     0,     0,     0,
       0,   349,     0,   350,     0,   220,     0,     0,   221,   157,
     158,   159,   160,     0,     0,   161,     0,   925,     0,     0,
       0,   926,     0,     0,     0,     0,   927,     0,     0,     0,
       0,     0,  1284,     0,     0,     0,   929,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   166,     0,
       0,     0,     0,   167,     0,     0,  1285,     0,     0,     0,
       0,   169,     0,     0,     0,   170,     0,     0,   171,   172,
       0,     0,     0,   173,     0,     0,   174,     0,   175,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   176,   177,     0,   178,   179,   180,   181,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   153,     0,
       0,     0,     0,     0,   931,   154,   155,     0,     0,     0,
       0,   156,     0,     0,     0,     0,     0,     0,     0,   190,
     191,   192,   193,   194,   195,   196,   197,   198,   199,   200,
     201,   202,   203,   204,   205,   206,   207,   208,   209,   210,
     211,     0,     0,   212,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   932,     0,     0,   933,   934,     0,     0,
       0,     0,     0,   935,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   157,   158,
     159,   160,     0,     0,   161,     0,   162,   213,   214,     0,
     163,     0,     0,   937,   938,   164,     0,     0,     0,     0,
       0,   165,     0,     0,     0,     0,   215,     0,     0,     0,
     216,   217,     0,     0,     0,     0,     0,   166,     0,     0,
     939,   940,   167,   758,     0,   168,     0,     0,     0,     0,
     169,     0,   941,     0,   170,   221,     0,   171,   172,     0,
       0,     0,   173,     0,     0,   174,     0,   175,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   176,   177,   153,   178,   179,   180,   181,     0,
       0,   154,   155,     0,     0,     0,     0,   156,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   182,   183,   184,   185,     0,     0,     0,
       0,     0,   186,   187,     0,     0,   188,   189,   190,   191,
     192,   193,   194,   195,   196,   197,   198,   199,   200,   201,
     202,   203,   204,   205,   206,   207,   208,   209,   210,   211,
       0,     0,   212,     0,   157,   158,   159,   160,     0,     0,
     161,     0,   162,     0,     0,     0,   163,     0,     0,     0,
       0,   164,     0,     0,     0,     0,     0,   165,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   166,     0,     0,   213,   214,   167,     0,
       0,   168,     0,     0,     0,     0,   169,     0,     0,     0,
     170,     0,     0,   171,   172,   215,     0,     0,   173,   216,
     217,   174,     0,   175,     0,     0,     0,     0,     0,     0,
       0,   218,   219,     0,     0,     0,     0,     0,     0,   350,
       0,   220,     0,     0,   221,     0,     0,     0,   176,   177,
       0,   178,   179,   180,   181,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,  1001,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   182,
     183,   184,   185,     0,     0,     0,     0,     0,   186,   187,
       0,     0,   188,   189,   190,   191,   192,   193,   194,   195,
     196,   197,   198,   199,   200,   201,   202,   203,   204,   205,
     206,   207,   208,   209,   210,   211,   921,     0,   212,     0,
       0,     0,   922,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   157,
     158,   159,   160,     0,     0,     0,     0,  1020,     0,     0,
       0,  1021,   213,   214,     0,     0,  1022,     0,     0,     0,
       0,     0,  1023,     0,     0,     0,   929,     0,     0,     0,
       0,   215,     0,     0,     0,   216,   217,     0,  1024,     0,
       0,     0,     0,  1025,     0,     0,  1026,   218,   219,     0,
       0,  1027,     0,     0,     0,  1028,     0,   220,  1029,  1030,
     221,     0,     0,  1031,     0,   153,  1032,     0,  1033,     0,
       0,     0,   154,   155,     0,     0,     0,     0,   156,  2189,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,  1034,     0,     0,  1035,     0,     0,  1036,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,  2190,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,  2191,     0,     0,     0,
       0,   161,     0,   162,     0,     0,     0,   163,     0,     0,
       0,     0,   164,     0,     0,     0,     0,     0,   165,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   932,   166,     0,   933,   934,     0,   167,
       0,     0,   168,   935,     0,     0,     0,   169,     0,     0,
       0,   170,     0,     0,   171,   172,     0,     0,     0,   173,
       0,     0,   174,     0,   175,     0,     0,     0,     0,     0,
       0,     0,     0,   937,   938,     0,  1037,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   176,
     177,   153,   178,   179,   180,   181,     0,     0,   154,   155,
     939,   940,     0,     0,   156,     0,     0,     0,     0,     0,
     350,     0,  1038,     0,     0,   221,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     182,   183,   184,   185,     0,     0,     0,     0,     0,   186,
     187,     0,     0,   188,   189,   190,   191,   192,   193,   194,
     195,   196,   197,   198,   199,   200,   201,   202,   203,   204,
     205,   206,   207,   208,   209,   210,   211,     0,     0,  2192,
       0,     0,     0,     0,     0,     0,     0,   161,     0,   162,
       0,     0,     0,   163,  2193,     0,     0,     0,   164,     0,
       0,     0,     0,     0,   165,     0,     0,     0,     0,     0,
       0,     0,  2194,     0,     0,     0,     0,  2195,     0,  2196,
     166,     0,     0,   213,   214,   167,     0,     0,   168,     0,
       0,     0,     0,   169,     0,     0,  2197,   170,     0,     0,
     171,   172,   215,     0,     0,   173,   216,   217,   174,     0,
     175,     0,     0,     0,     0,     0,     0,     0,   218,   219,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   176,   177,     0,   178,   179,
     180,   181,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   182,   183,   184,   185,
       0,     0,     0,     0,     0,   186,   187,     0,     0,   188,
     189,   190,   191,   192,   193,   194,   195,   196,   197,   198,
     199,   200,   201,   202,   203,   204,   205,   206,   207,   208,
     209,   210,   211,     0,     0,   212,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     2,     3,     0,     0,     0,     4,     0,
       5,     0,     0,     0,     0,     0,     0,     0,     0,   213,
     214,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       6,     7,     8,     9,     0,     0,     0,     0,   215,     0,
       0,    10,   216,   217,     0,     0,     0,     0,     0,    11,
       0,    12,     0,    13,   218,   219,    14,    15,    16,     0,
      17,     0,     0,     0,    18,    19,    20,     0,    21,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    34,     0,     0,     0,     0,   921,     0,     0,     0,
       0,     0,   922,     0,     0,   325,   326,   327,  1886,  1887,
     330,     0,     0,     0,     0,     0,     0,     0,  -438,     0,
       0,     0,     0,     0,    35,     0,     0,     0,     0,   157,
     158,   159,   160,     0,    36,     0,     0,  1020,     0,     0,
      37,  1021,     0,     0,     0,     0,  1022,     0,     0,     0,
       0,     0,  1023,     0,     0,     0,   929,     0,     0,     0,
       0,     0,     0,     0,    38,     0,     0,     0,  1024,     0,
       0,     0,     0,  1025,     0,     0,  1026,   921,     0,     0,
       0,  1027,     0,   922,     0,  1028,     0,     0,  1029,  1030,
       0,     0,     0,  1031,     0,     0,  1032,     0,  1033,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     157,   158,   159,   160,     0,     0,     0,     0,  1020,     0,
       0,     0,  1021,  1034,     0,     0,  1035,  1022,     0,  1036,
       0,     0,     0,  1023,     0,     0,     0,   929,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,  1024,
       0,     0,     0,     0,  1025,     0,     0,  1026,     0,     0,
       0,     0,  1027,     0,     0,     0,  1028,     0,     0,  1029,
    1030,     0,     0,     0,  1031,     0,     0,  1032,     0,  1033,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    39,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,  1034,     0,    40,  1035,     0,     0,
    1036,  -438,     0,   932,     0,     0,   933,   934,     0,     0,
       0,     0,     0,   935,    41,     0,     0,     0,     0,    42,
       0,    43,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   937,   938,     0,  1037,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   921,     0,     0,     0,
       0,     0,   922,     0,     0,     0,     0,     0,     0,     0,
     939,   940,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,  1038,     0,   932,   221,     0,   933,   934,  1714,
     158,   159,   160,     0,   935,     0,     0,  1020,     0,     0,
       0,  1021,     0,     0,     0,     0,  1022,     0,     0,     0,
       0,     0,  1023,     0,     0,     0,   929,     0,     0,     0,
       0,     0,     0,     0,   937,   938,     0,  1037,  1024,     0,
       0,     0,     0,  1025,     0,     0,  1026,     0,     0,     0,
       0,  1027,     0,     0,     0,  1028,     0,     0,  1029,  1030,
       0,   939,   940,  1031,     0,     0,  1032,     0,  1033,     0,
       0,     0,     0,  1038,     0,     0,   221,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,  1034,     0,     0,  1035,     0,     0,  1036,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   932,     0,     0,   933,   934,     0,     0,
       0,     0,     0,   935,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   937,   938,     0,  1037,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     939,   940,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,  1038,     0,     0,   221
};

static const yytype_int16 yycheck[] =
{
       1,    38,     3,   452,   389,    68,   329,   519,   216,   353,
     363,    37,    53,     1,   443,    68,   440,  1031,   720,   408,
     877,   921,    30,   442,    65,    53,   687,   220,   396,   179,
     569,   631,   759,   368,   483,   441,   408,    65,   318,  1462,
      32,  1462,   870,   472,  1468,    53,    47,   419,   420,  1467,
     480,  1589,  1590,   570,    55,  1469,   369,    65,   479,    60,
     107,   423,   109,   469,   111,   112,  1409,   347,   115,  1483,
     117,  1466,   935,   120,   121,   994,   482,   124,   125,   126,
     127,   598,  1408,   843,   131,  1480,   439,   134,   518,   606,
    1323,   408,   139,   140,  1321,  1322,   517,   386,  1636,   388,
     147,  1328,   382,  1323,   534,   622,    36,  1323,   180,   462,
    1461,  1462,  1036,  1308,  1579,   875,  1323,  1622,   177,  1801,
     880,   180,  1799,   529,  1757,  1323,  1800,   177,   941,    30,
     180,    30,    30,   860,   447,  1005,  1006,   177,  1774,   899,
     180,  1774,   177,   903,  1008,   180,     7,     8,     9,  1007,
     667,  1778,    53,  1567,    53,    53,  1701,  1558,  1559,  1009,
     177,     5,   190,   180,    65,   478,    65,    65,   177,  1011,
       8,   180,  1983,   177,   177,    24,   180,   180,    16,    17,
      18,    19,     8,   177,   177,    39,   180,   180,    49,  1416,
      51,     5,  2015,   177,   711,    24,   180,     8,   177,   512,
      61,   180,    58,   177,   177,    66,   180,   180,   216,    70,
     177,    21,   220,   180,   177,     8,     8,   180,   531,  1592,
     177,  1759,  1760,   180,   104,    80,    81,    86,     1,   746,
     837,   838,     5,    98,     7,   840,     8,  1775,  1776,    24,
     104,   723,   365,   725,   756,   727,   128,   370,  1825,   107,
     732,   109,   784,   111,    27,    28,    29,    30,   105,   117,
     842,     9,   120,    81,   843,   328,   329,    91,    13,    42,
     408,   783,    45,  1292,  1600,  1601,   134,  1700,   105,    97,
     272,   419,   420,   105,   106,   107,   278,    23,   295,   147,
     122,   105,   106,   106,  2046,   105,   841,    50,   116,   135,
    1034,   137,    33,    34,    35,   333,   115,    55,  1358,    59,
    1360,   793,   312,   825,    86,   216,   128,   216,   216,   220,
    1633,   220,   220,   135,    86,   353,   408,   136,    30,   105,
     106,  1091,   864,   396,     9,    58,    63,   419,   420,    91,
     149,   105,    94,    88,   145,   353,   127,    50,  2115,  1700,
      58,    53,     5,   165,     7,   408,    30,   294,   365,   115,
     221,   162,   811,    65,    54,   365,   419,   420,   104,   105,
     151,    48,  1033,   100,  1797,   312,  1797,    59,   441,    53,
      55,   162,   105,   105,   105,   106,   107,    40,   441,    58,
     819,    65,   165,    91,   824,   436,    94,   105,    93,     5,
    1633,     7,   823,   165,    12,  1638,   469,     8,   436,    30,
     451,   817,   294,  1633,   105,   112,   469,  1633,  1638,   482,
      21,  1772,  1638,   451,  1024,    91,  1633,  1027,   436,   482,
     127,  1638,    53,     1,  1785,  1633,   105,     5,   133,     7,
    1638,   105,   364,   451,    65,  1796,  1797,  1289,   364,   285,
     314,   298,   353,  1187,   353,   353,   503,   289,   282,    27,
      28,    29,    30,    86,   105,    73,   529,   328,   329,    77,
     288,   332,   299,   332,   333,   334,   529,  1084,  1085,  1086,
    2232,     6,  2115,  1088,    92,    93,   296,  1506,  2124,   365,
     370,  2124,   348,   349,  2317,  1102,  2263,     8,   307,  1872,
     365,     1,   110,   259,   260,     5,  2044,     7,  1090,   822,
      21,   365,  1091,  2324,   216,   370,   365,     5,   220,     7,
     293,   370,   366,   370,   255,   133,   134,    27,    28,    29,
      30,   369,   296,  1293,  1294,   436,   144,   436,   436,    39,
     366,   370,   216,   151,  1089,  1387,   220,  2092,   370,   699,
     451,     9,   451,   451,  2190,   366,   370,  2190,    16,    17,
      18,    19,  2139,  2245,   336,    23,  2243,  1437,  1438,   954,
    2244,   597,  1385,   366,   366,   298,  1440,   295,   359,  2206,
     365,  1439,  1933,   620,    91,   370,  2091,  1900,  1931,   284,
     298,  1441,   453,   366,   370,   216,   457,    55,   350,   220,
     461,   351,   352,   353,  1143,   466,  1145,   298,    91,     1,
     471,  1661,  1129,     5,  1664,     7,  2081,  1667,   132,   285,
    1069,   287,   136,    30,  1548,    58,   362,   365,   700,   298,
    2263,  2032,   296,    46,   370,    27,    28,    29,    30,   698,
      38,   700,   850,  1011,   365,   503,    53,   365,   937,   938,
     700,   353,   350,   583,  1261,   169,   170,   298,    65,   244,
     700,  1888,   870,   698,   525,   700,  1038,  1900,  1057,   351,
     352,   353,   105,   134,    91,   283,   284,    94,   715,   353,
    1900,   698,   543,   700,  1900,  1057,   712,  1882,  1883,   698,
      88,   700,    91,  1900,   698,  1012,   700,   700,  1359,  1618,
      58,    86,  1900,  2029,   698,   698,   700,   700,  1935,   332,
     333,   334,   314,   315,   698,  1077,   700,   145,    91,   698,
       6,   700,   150,   115,   698,   698,   700,   700,   147,    46,
      47,   698,   353,   700,   436,   698,  1218,   700,   115,   877,
    1057,   698,   127,   700,   312,  2169,  2089,   105,  1037,   451,
    2168,   244,   365,   278,   817,   132,  2170,   370,    86,   129,
      88,  2299,   436,   365,   817,  1622,   151,  1630,   293,   129,
    1282,   343,  2167,   298,    86,    86,    88,   451,   285,  1114,
     287,   112,   350,  1640,   154,    86,   311,    88,   165,  1617,
     278,   316,    39,    40,   154,   877,   127,   304,   366,  1379,
    1380,  1340,   285,   941,   287,   293,  1386,   870,     1,   216,
     298,   352,     5,   220,     7,   436,   127,  1010,  2241,   104,
    2241,  1581,   332,   311,   877,     1,   169,   170,   316,     5,
     451,     7,   342,   104,    27,    28,    29,    30,     9,  1288,
     151,   366,   850,   835,  1701,    16,    17,    18,    19,  1279,
     350,    27,    28,    29,    30,  1276,   294,   276,   277,    79,
     104,   278,   870,  1565,   173,   298,   366,  1005,  1006,  1007,
    1008,  1009,   361,   362,  1012,  1305,   293,   352,   105,  1606,
    1607,   298,   115,  1304,    55,  1035,   285,   279,   287,  2240,
    2241,   168,   112,   930,   311,   293,   173,   294,    91,   316,
    1038,     9,   928,   130,   131,    98,   294,   127,  1243,  1244,
     105,   369,   285,   311,   287,   523,   244,    25,   316,  1057,
     318,  1289,    86,  1005,  1006,  1007,  1008,  1009,   104,   350,
    1012,   994,   244,   350,     5,  2162,   169,   170,  1599,   358,
     298,   188,   135,   244,   191,   178,   353,    55,  1011,   850,
     317,   850,   850,   173,   295,   296,  1038,   143,   350,  1012,
    1273,   165,    50,    51,  1476,    53,    54,    55,    18,   870,
     156,   870,   870,   154,   366,  1057,  1256,   170,   306,   326,
     136,    38,  1499,    19,   165,   166,  1647,   173,   369,    46,
    1303,    48,   278,    50,   306,    53,    53,    54,    55,    21,
      57,    23,  1010,    10,  1057,   306,   136,   293,  1957,  1958,
    1959,  1960,   298,   169,   170,  1464,   172,  1402,    53,  1387,
     104,   105,  1922,  2250,    21,   311,    23,  1544,   255,   436,
     316,   115,   365,    40,    41,    42,    43,   113,   114,   169,
     170,   365,   262,     6,   451,  1475,     9,  1436,   275,   276,
       0,   314,   315,  1474,  1443,     5,     1,     7,   104,   105,
       5,  1062,     7,  1435,  1436,  1066,   314,   315,    75,   115,
    1442,  1443,  1444,  1445,   365,   205,    83,    84,    85,    86,
     366,   365,    27,    28,    29,    30,   279,     9,  1296,     5,
     127,     7,   365,     9,    16,    17,    18,    19,    58,   106,
       8,    23,   306,   307,   308,   309,   310,   295,   238,  1010,
     147,  1010,  1010,    21,   151,    23,   123,    44,  1144,   365,
    1146,   128,   129,   130,     8,   344,  1152,  2141,   347,  1831,
       9,   138,   115,    55,   141,     9,   165,    46,   314,   315,
    1001,   327,    16,    17,    18,    19,   129,    46,   850,  1484,
     104,   105,  1487,   136,  2103,  2104,  2105,   350,    46,     1,
    1473,   115,     8,     5,    23,     7,     1,    26,   870,     8,
       5,   154,     7,   366,   350,    21,   850,    23,   161,   255,
     256,    55,    21,   259,    23,    27,    28,    29,    30,   310,
     366,   105,    27,    28,    29,    30,   870,   115,   369,   104,
      42,  1409,  1632,    45,   115,   293,   117,   118,   119,   120,
    1631,   371,   300,   301,   132,   343,   245,   246,   247,   137,
     138,   278,  1736,  1737,  1738,  1739,  1289,   371,   316,   850,
    1747,     6,   343,  1568,     9,  2092,   293,    16,    17,   276,
     277,  1379,  1380,  1381,  1382,  1383,   228,  1385,  1386,   870,
     232,   233,   234,   104,   311,   360,  1394,  1395,     1,   316,
    1604,   318,     5,     1,     7,   337,   338,     5,   239,     7,
     105,   106,   107,  1608,  1609,  1724,  1611,   248,   249,    16,
      17,   252,   253,   368,    27,    28,    29,    30,  1296,    27,
      28,    29,    30,  1628,   301,   302,   303,  1435,  1436,  1437,
    1438,  1439,  1440,  1441,  1442,  1443,  1444,  1445,  1010,  1694,
     105,  1696,    86,   108,   109,   110,  1629,    63,    64,    16,
      17,   358,    68,   165,  1387,   178,   244,    61,    62,    24,
    1719,  1720,   195,   196,  1387,    24,  1010,    53,    80,    81,
      82,    83,    84,    85,  1856,     1,    53,  1719,  1720,     5,
     295,     7,    95,  1435,  1436,  1437,  1438,  1439,  1440,  1441,
    1442,  1443,  1444,  1445,   113,   114,   115,   105,   106,   107,
      53,    27,    28,    29,    30,   193,   194,   111,   320,   321,
     322,   323,   324,    39,   113,   114,   117,   121,     5,  1010,
       7,     8,   135,   127,   129,  1296,    53,  1296,  1296,   113,
     114,  1409,   133,  1920,  1921,   350,  1923,     1,    53,  1617,
     145,     5,    86,     7,    87,    88,    89,   151,    91,   154,
    1850,   366,   365,   154,  1845,   104,   161,   162,   104,   278,
     161,    86,  1460,    27,    28,    29,    30,   444,   445,   446,
    1954,  1955,  1956,   850,   293,     5,     6,   369,    86,   298,
      18,   293,  1460,   113,   114,  1456,  1457,   104,  1459,  1460,
    1461,  1462,   311,   870,   351,   352,   353,   316,   104,  1457,
     477,  1459,  1460,  1461,  1462,  1992,  1789,   104,   105,  1505,
     104,  2142,  2082,  2083,  1622,   139,   140,   141,   142,  2249,
     104,    46,   104,    48,   104,   369,    96,    97,    53,    19,
      55,   369,  1640,   510,    59,   336,   337,   338,  1409,     6,
    1409,  1409,   105,   106,   107,   350,     8,  1554,  1555,   250,
     251,  1950,     8,  1947,   366,  1838,  1552,  1553,     8,  1948,
    2290,   366,   223,   224,   225,   226,   279,    44,    45,     8,
    1622,     1,  1877,    30,    99,     5,   101,     7,   332,   333,
     334,   104,  1615,   104,  1617,  1618,   104,   105,  1640,  1460,
       6,  1460,  1460,  1701,  1887,     8,    53,    27,    28,    29,
      30,     8,  1573,   365,  1980,  1576,  1604,  1578,    65,    19,
    1581,  1719,  1720,    18,   369,  1573,     9,  1640,  1576,   104,
    1578,   136,  1945,  1581,  1296,     8,  1604,  1394,  1395,   104,
     145,  1642,  2008,  1010,   365,    39,   365,   350,   365,  1617,
     365,   365,   350,   295,  1642,   104,   161,   365,  1644,  1701,
     165,   104,  1296,   366,   169,   170,   312,   104,   366,   174,
     175,   176,   177,   178,  1642,   365,   104,  1719,  1720,     1,
     104,  1642,   105,     5,    98,     7,   104,   104,   192,   232,
      30,   197,     1,   132,   235,   115,     5,   235,     7,   236,
     113,   114,  1870,   237,   236,    27,    28,    29,    30,   344,
       6,   104,   104,    53,   127,  1296,   115,   104,    27,    28,
      29,    30,   135,  1746,  2196,    65,   104,   104,   127,   104,
     129,     9,   104,    42,   350,   365,    45,   365,   151,   365,
     365,   295,   104,  1604,  2039,  1604,  1604,  1409,   147,    53,
     366,    23,   151,    44,  2049,   154,  1617,   313,  1617,  1617,
     150,  2056,   313,  1931,    21,     6,  2070,   368,   104,   216,
      86,     9,    86,   220,    47,  1409,    49,    23,    51,    52,
    2189,  1642,  1743,  1642,  1642,   368,    59,    60,  2265,  1757,
      23,  1752,  1753,  1754,  1755,  1756,   350,   104,  1460,   365,
     115,   365,   117,   365,     8,  2195,  1774,  1795,  1769,  1770,
    1771,  1772,   366,  2194,   372,   372,   104,   296,  1806,  2296,
      93,   136,  1783,  1784,  1785,  2302,  1460,  1795,  1409,   144,
     145,   146,   147,  1794,  1795,  1796,  1797,     6,  1806,  2134,
    2135,  2136,  2314,   105,   243,  1806,  1794,  1795,  1796,  1797,
      44,   365,   312,   192,   821,   105,   165,     8,  1806,   279,
      21,   104,   104,  1886,  1887,    71,   279,   280,   281,   282,
       6,     8,   317,  1834,  1835,  1836,   216,   276,   277,  1460,
     220,   167,   295,   296,   297,   104,  1834,  1835,  1836,  1324,
      40,   105,   364,   100,  2303,   364,  1757,   105,  1757,  1757,
     364,   104,  1870,   364,   100,   104,   353,     8,   366,  1730,
    1731,  1732,  1733,  1774,   104,  1774,  1774,   104,   104,   295,
    2193,  2089,    86,  2313,    99,   100,   101,   102,     6,  1296,
     350,  2312,   191,   104,  1795,     8,  1795,  1795,   105,  1962,
     113,   114,  1604,  1904,     9,  1806,   366,  1806,  1806,   122,
     123,   124,   125,   126,     9,  1617,   104,  1980,   104,   358,
     152,   408,  2257,  1931,   104,   106,   364,  1980,   188,   365,
    1604,    42,   419,   420,   227,   228,   229,   230,   231,   365,
    1642,   366,     8,  1617,   293,  2008,   365,   105,   135,   436,
     312,   115,  2015,     9,  2092,  2008,   365,   121,   365,   365,
     124,   125,   126,   127,   451,   365,   365,   131,  1642,  1870,
     365,  1870,  1870,   353,   365,   139,   140,  1978,  1979,   366,
     366,   365,  1457,  1604,  1459,  1460,  1461,  1462,   350,     9,
     365,  2038,  2327,  2328,  2329,   365,  1617,  1998,  2311,   105,
     104,  2048,  1409,   104,   366,  2006,  2007,   104,  2055,  2010,
    2092,     6,   364,  1028,  1030,  2016,   922,   366,   796,  1029,
     797,  1642,  2023,  2024,  2025,  1886,  1887,   795,   408,   794,
    1931,  1137,  1931,  1931,   575,  2023,  2024,  2025,  1492,   419,
     420,   565,  2070,  1032,    20,   282,   134,  2110,   283,   438,
    2102,  1465,  2122,  1460,    46,  1757,   436,  2128,  2250,  1779,
    2120,  2175,  2070,  2178,  2177,  2176,   281,  2130,  2166,    61,
      62,   451,  1774,  2292,  1732,   290,   291,   292,   293,   892,
     288,  2089,   435,  1757,  1935,   904,  1906,  1855,   377,  1274,
    1575,  1575,  1843,  1795,  1654,  2096,   977,  1844,  1573,  1656,
    1774,  1576,   437,  1578,  1806,  2237,  1581,  2115,  2165,  1381,
    2111,  1400,  1382,   105,  1383,   997,  2124,  1615,  1617,   111,
     871,  1795,   320,    30,  1407,  1128,  1445,   216,  1328,   121,
    2131,  2132,  1806,  1870,  1605,   127,  1757,  1638,   447,   449,
     420,  1638,  2143,  2144,   450,  2146,  2147,  2148,  2149,  2150,
       1,  2300,  2153,  1774,     5,   333,     7,   448,   721,   151,
     334,   334,  2220,  1638,   698,  1196,   665,    -1,  1870,  2070,
      -1,  2070,  2070,    -1,  1795,    -1,    27,    28,    29,    30,
       1,    -1,  2190,    -1,     5,  1806,     7,  2250,  2089,    -1,
    2089,  2089,    -1,    -1,    -1,    -1,  1870,  1604,  2199,    -1,
      -1,  2202,    -1,    -1,  2205,    -1,    27,    28,    29,    30,
    1617,  2239,    -1,    -1,  2115,    -1,  2115,  2115,    -1,    -1,
      -1,    42,    -1,  2124,    45,  2124,  2124,  2290,    -1,  1931,
      -1,  2239,    -1,    -1,    -1,  1642,    -1,  2238,  2239,  2240,
    2241,    -1,    -1,    -1,    -1,  2246,  2247,  2248,  2249,  1870,
    2238,  2239,  2240,  2241,  2317,  2263,    -1,  1931,  2246,  2247,
    2248,  2249,    -1,    -1,   115,  2266,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,  2274,    -1,  2276,  2277,  2278,  2279,  2280,
    2281,  2282,  2283,  2284,  2285,  2286,  2287,    -1,    -1,  2190,
      -1,  2190,  2190,  2281,  2282,  2283,    -1,     1,    -1,    -1,
    2301,     5,    -1,     7,    -1,    -1,   298,  2308,  2309,  2310,
    1931,    14,    15,    16,    46,    -1,    48,    -1,    -1,  1794,
    1795,  1796,  1797,    27,    28,    29,    30,    -1,     1,    61,
      62,  1806,     5,    -1,     7,    -1,    -1,    -1,  2239,    -1,
    2239,  2239,    -1,    -1,   165,    -1,    -1,    -1,    -1,    -1,
    1757,    -1,    -1,    -1,    27,    28,    29,    30,    -1,  1834,
    1835,  1836,  2263,   850,  2263,  2263,    -1,  1774,  2070,    42,
      -1,    -1,    45,   105,    -1,    78,    79,    80,    -1,   111,
       1,    -1,    -1,   870,     5,    -1,     7,  2089,  1795,   121,
     877,    95,    95,    96,    -1,   127,  2070,    -1,    -1,  1806,
      -1,   104,    -1,    -1,    -1,    -1,    27,    28,    29,    30,
     113,   114,    -1,  2115,    -1,  2089,   119,    38,    -1,   151,
      -1,    -1,  2124,    -1,    -1,    -1,    -1,    48,   279,    -1,
      -1,   135,    53,   136,    55,    -1,    57,    -1,    -1,    -1,
      -1,  2115,   145,   146,    -1,    -1,   149,    -1,    -1,  2070,
    2124,    -1,    -1,    -1,   941,     1,    -1,    -1,    -1,     5,
      -1,     7,    -1,  1870,    -1,    -1,    -1,    -1,  2089,    -1,
     850,    -1,   293,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    27,    28,    29,    30,    -1,    -1,    -1,  2190,    -1,
     870,    -1,   165,    -1,  2115,    -1,    -1,   877,    -1,   350,
      -1,     1,    -1,  2124,    -1,     5,     1,     7,    -1,    -1,
       5,    -1,     7,    -1,    -1,   366,  2190,    -1,  1005,  1006,
    1007,  1008,  1009,  1010,  1931,  1012,    -1,    27,    28,    29,
      30,    -1,    27,    28,    29,    30,    -1,  2239,    -1,    -1,
      -1,    41,    -1,    -1,   165,   366,    -1,    -1,  2023,  2024,
    2025,  1038,    -1,    -1,    -1,    -1,    -1,    -1,   104,    -1,
      -1,  2263,    -1,    -1,    -1,  2239,   298,    -1,    -1,  2190,
    1057,    -1,    -1,    -1,    -1,   279,    -1,    -1,    -1,    -1,
     847,     1,   849,   286,   287,     5,    -1,     7,    -1,  2263,
      90,    91,    92,    93,    -1,    95,    91,    -1,    98,   866,
     867,    -1,   869,    98,    -1,   872,   873,    27,    28,    29,
      30,    -1,    -1,    -1,   994,   115,    -1,    -1,  2239,    -1,
     293,    -1,    -1,    -1,    -1,  1005,  1006,  1007,  1008,  1009,
    1010,    -1,  1012,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     135,     1,  2263,    -1,    -1,     5,   350,     7,    -1,    -1,
      -1,    -1,    -1,   920,    -1,    -1,   923,   278,  1038,    -1,
      -1,    -1,   366,  2070,    -1,   165,    -1,    27,    28,    29,
      30,    91,   293,    -1,    -1,   170,    -1,  1057,    98,    -1,
      -1,    -1,  2089,   113,   114,    -1,    -1,   117,    -1,    -1,
     311,    -1,    -1,   366,    -1,   316,    -1,   318,   128,   129,
     130,   131,   132,   133,   134,   135,   136,    -1,  2115,   114,
     115,    -1,   117,    -1,    -1,   135,    -1,  2124,    -1,   113,
     114,   988,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   350,
      -1,   136,    -1,    -1,   128,    95,   130,   131,   132,   144,
     145,    -1,   147,    -1,    -1,   366,    -1,    -1,   178,    -1,
     170,    -1,   157,   158,   159,    -1,    -1,    -1,    -1,   153,
     154,   155,    -1,  2238,  2239,  2240,  2241,    -1,   314,   315,
      -1,  2246,  2247,  2248,  2249,   135,    -1,    -1,   278,    -1,
      -1,    -1,    -1,  2190,   279,    -1,    -1,    -1,    -1,    -1,
      -1,  1058,  1059,   293,    -1,    -1,    -1,    -1,   298,   502,
      -1,    -1,    -1,    -1,   350,    -1,  2281,  2282,  2283,  1296,
      -1,   311,    -1,    -1,    -1,    -1,   316,    -1,    -1,    -1,
     366,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,  2239,    -1,    -1,    -1,    -1,    -1,   541,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,     1,    -1,    -1,
     350,     5,    -1,     7,    -1,   350,  2263,    -1,    -1,   279,
      -1,    -1,    -1,    -1,    -1,    -1,   366,    -1,    -1,    -1,
      -1,   366,    -1,    27,    28,    29,    30,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    41,    -1,    -1,
      -1,    -1,  1379,  1380,  1381,  1382,  1383,    -1,  1385,  1386,
      -1,    -1,    -1,    -1,    -1,     1,    -1,  1394,  1395,     5,
      38,     7,    -1,    -1,    -1,    -1,  1296,    -1,    46,   279,
      48,    -1,  1409,    -1,    -1,    53,    54,    55,    -1,    57,
     350,    27,    28,    29,    30,    -1,    90,    91,    92,    93,
      -1,    95,    -1,    -1,    98,    41,   366,    -1,  1435,  1436,
    1437,  1438,  1439,  1440,  1441,  1442,  1443,  1444,  1445,    -1,
      -1,   115,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,  1460,    -1,    -1,    -1,  1357,  1358,    -1,
    1360,     1,    -1,    -1,    -1,     5,    -1,     7,    -1,    -1,
     350,    -1,    -1,    -1,    90,    91,    92,    93,    -1,    95,
      -1,    -1,    98,    -1,    -1,    -1,   366,    27,    28,    29,
      30,   165,    24,    -1,    -1,    -1,  1283,    -1,    -1,   115,
    1287,    41,    -1,    -1,    -1,    -1,    -1,    39,    -1,  1409,
    1297,  1298,  1299,  1300,    -1,  1302,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    71,    -1,
      -1,  1318,    -1,    -1,    77,  1435,  1436,  1437,  1438,  1439,
    1440,  1441,  1442,  1443,  1444,  1445,    -1,    -1,    -1,   165,
      -1,    91,    92,    93,    -1,    95,    -1,    -1,    98,    -1,
    1460,    -1,   105,   106,   107,    -1,    -1,    -1,    -1,   112,
      -1,    -1,   104,   116,    -1,   115,    -1,  1364,   121,  1366,
      -1,    -1,    -1,    -1,   127,    -1,  1373,    -1,   131,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,  1604,    -1,    -1,
      -1,  1388,    -1,  1390,   278,    -1,    -1,    -1,   151,    -1,
    1617,    -1,  1399,    -1,  1401,  1622,    -1,    -1,    -1,   293,
      -1,    -1,    -1,    -1,   298,   165,    -1,    -1,    -1,    -1,
     278,    -1,    -1,  1640,    -1,  1642,    -1,   311,    -1,    -1,
      -1,    -1,   316,    -1,   176,   293,    -1,   179,   180,   181,
     182,   183,   184,   185,   186,   187,   188,   189,   190,   191,
      -1,    -1,   278,   311,    -1,    -1,    -1,    -1,   316,    -1,
     318,    -1,    -1,    -1,    -1,    -1,   350,   293,    -1,    -1,
      -1,    -1,   298,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   366,    -1,  1701,   311,    -1,    -1,    -1,    -1,
     316,    -1,    -1,    -1,  1604,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,  1719,  1720,    -1,  1615,    -1,  1617,  1618,    -1,
      -1,    -1,  1622,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   350,    -1,    -1,    -1,   278,    -1,
    1640,    -1,  1642,    -1,    -1,   288,    -1,    -1,   291,   292,
     366,    -1,    -1,   293,    -1,   298,    -1,    -1,   298,    -1,
      -1,  1661,    -1,    -1,  1664,    -1,    -1,  1667,    -1,    -1,
      -1,   311,    -1,    -1,    -1,    -1,   316,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   328,   329,    -1,  1795,    -1,
      -1,    -1,    38,    -1,    -1,    -1,    -1,  1584,  1585,  1806,
      46,  1701,    48,    -1,    50,    -1,    -1,    53,    54,    55,
     350,    57,    -1,    -1,    -1,    61,    62,    -1,    -1,  1719,
    1720,    -1,    -1,  1610,    -1,    -1,   366,   370,    -1,    -1,
      -1,    -1,  1619,    -1,    -1,    -1,    -1,    -1,    -1,     1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    11,
      12,    13,    14,    15,    16,    17,    -1,    -1,    20,    -1,
      -1,    -1,    -1,  1870,    -1,   111,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   121,    -1,    -1,    -1,    -1,
      -1,   127,    -1,  1670,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,  1795,    -1,    -1,    -1,    -1,
    1687,    -1,  1689,    -1,    -1,   151,  1806,    -1,    -1,    -1,
      -1,    73,    -1,    -1,    76,    77,    78,    79,    80,    81,
      -1,    -1,    -1,    -1,  1931,    -1,    88,    -1,    90,    91,
      92,    93,    94,    95,    96,    97,    -1,    99,   100,   101,
     102,    -1,   104,    -1,    -1,    -1,   108,    -1,   110,    -1,
     112,   113,   114,   115,   116,    -1,    -1,   119,    -1,   121,
      -1,    -1,   124,   125,   126,   127,    -1,    -1,    -1,   131,
    1870,   133,   134,   135,   136,   137,    -1,   139,   140,    -1,
      -1,   143,   144,   145,   146,    -1,    -1,   149,    -1,   151,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    38,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    46,
      -1,    48,    -1,    50,    -1,     1,    53,    54,    55,     5,
      57,     7,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,  1931,   278,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    27,    28,    29,    30,    -1,    -1,   293,    -1,    -1,
      -1,    -1,    38,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    48,  2070,    -1,   311,    -1,    53,    -1,    55,
     316,    57,   318,    -1,    -1,    -1,  1863,    -1,    -1,    -1,
      -1,    -1,  2089,    -1,    -1,  2092,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    38,    -1,    -1,  1885,    -1,
      43,    -1,    -1,    46,    47,    48,    49,    -1,    -1,    52,
      53,    -1,    55,    56,    57,    -1,    59,    -1,    -1,   281,
     282,   283,   284,   285,   286,   287,   288,    -1,   290,   291,
     292,   293,    -1,    -1,    -1,    -1,    -1,    -1,  1925,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,     1,    -1,    -1,    -1,
       5,    -1,     7,    -1,    -1,    -1,    99,    -1,   101,   102,
     103,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    24,
    2070,    -1,    27,    28,    29,    30,    31,    32,    -1,   165,
      -1,    36,    37,    38,    -1,    -1,    -1,    -1,    -1,  2089,
      45,    -1,  2092,    -1,    -1,    -1,    -1,    -1,    53,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    63,    64,
      -1,    -1,    -1,    68,    -1,    -1,    71,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    80,    81,    82,    83,    84,
      85,    -1,  2239,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      95,   278,    -1,    -1,    99,    -1,   101,    -1,    -1,   104,
     105,   106,   107,    -1,    -1,   110,   293,   112,    -1,    -1,
     115,   116,    -1,    -1,    -1,    -1,   121,    -1,    -1,    -1,
      -1,    -1,   127,    -1,   311,    -1,    -1,    -1,    -1,   316,
      -1,   318,    -1,    -1,    -1,    -1,    -1,    -1,   143,    -1,
      -1,    -1,   278,   148,    -1,    -1,   151,    -1,    -1,    -1,
    2087,   156,    -1,    -1,    -1,   160,    -1,   293,   163,   164,
     165,    -1,    -1,   168,    -1,    -1,   171,    -1,   173,    -1,
      -1,    -1,    -1,    -1,    -1,   311,    -1,    -1,    -1,   491,
     316,    -1,   318,    -1,    -1,   278,    -1,    -1,    -1,  2239,
     502,    -1,    -1,   198,   199,    -1,   201,   202,   203,   204,
     293,    -1,    -1,    -1,    -1,    -1,    -1,   300,   301,   302,
     303,   523,    -1,    -1,   350,    -1,    -1,    -1,   311,    -1,
      -1,    -1,    -1,   316,    -1,   318,   319,    -1,    -1,   541,
     366,    -1,    -1,    -1,   239,   240,   241,   242,    -1,    -1,
      -1,    -1,    -1,   248,   249,    -1,    -1,   252,   253,   254,
     255,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,    -1,    -1,   278,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   293,    -1,
      -1,    -1,    -1,   298,    -1,    -1,    -1,    -1,    -1,    -1,
     305,    -1,    -1,    -1,    -1,    -1,   311,    -1,    -1,    -1,
      -1,   316,    -1,    -1,    -1,    -1,    -1,   322,   323,    -1,
     325,    -1,    -1,   328,   329,   330,   331,    -1,    -1,    -1,
     335,    -1,    39,    -1,   339,   340,   341,    -1,    -1,    -1,
     345,   346,    -1,    -1,     1,   350,    -1,    -1,     5,   354,
       7,    -1,   357,   358,    -1,    -1,    -1,    -1,   363,    -1,
     365,   366,   367,    -1,    -1,   370,    -1,    24,    -1,    -1,
      27,    28,    29,    30,    31,    32,    -1,    -1,    -1,    36,
      37,    38,    -1,    -1,    -1,    -1,    -1,    -1,    45,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    53,   104,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    63,    64,    -1,    -1,
     105,    68,    -1,    -1,    71,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    80,    81,    82,    83,    84,    85,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   132,    95,    -1,
     135,    -1,    99,    -1,   101,    -1,    -1,   104,   105,   106,
     107,    -1,    -1,   110,    -1,   112,    -1,   152,   115,   116,
      -1,    -1,    -1,    -1,   121,    -1,    -1,    -1,    -1,   176,
     127,    -1,   179,   180,   181,   182,   183,   184,   185,   186,
     187,   188,   189,   190,   191,    -1,   143,    -1,    -1,    -1,
      -1,   148,    -1,    -1,   151,   183,    -1,    -1,    -1,   156,
      -1,    -1,    -1,   160,    -1,    -1,   163,   164,   165,    -1,
      -1,   168,    -1,    -1,   171,    -1,   173,    -1,   206,   207,
     208,   209,   210,   211,   212,   213,   214,   215,   216,   217,
     218,   219,   220,   221,   222,    -1,    -1,    -1,    -1,    -1,
      -1,   198,   199,    -1,   201,   202,   203,   204,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     255,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,    -1,   239,   240,   241,   242,    -1,    -1,    -1,    -1,
      -1,   248,   249,    -1,    -1,   252,   253,   254,   255,   256,
     257,   258,   259,   260,   261,   262,   263,   264,   265,   266,
     267,   268,   269,   270,   271,   272,   273,   274,   275,    -1,
      -1,   278,    -1,   318,    -1,    -1,    -1,   322,   323,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   293,   332,    -1,    -1,
      -1,   298,    -1,    -1,    -1,    -1,    -1,    -1,   305,    -1,
      -1,    -1,    -1,    -1,   311,    -1,    -1,    -1,    -1,   316,
      -1,    -1,    -1,   358,    -1,   322,   323,    -1,   325,    -1,
      -1,   328,   329,   330,   331,    -1,    -1,    -1,   335,    -1,
      -1,    -1,   339,   340,   341,    -1,    -1,    -1,   345,   346,
      -1,    -1,    -1,   350,    -1,    -1,    -1,   354,    -1,    91,
     357,   358,    -1,    -1,    -1,    -1,   363,    -1,   365,   366,
     367,     1,    -1,   370,    -1,     5,    -1,     7,    -1,    -1,
      -1,   113,   114,    -1,    -1,   117,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    24,    -1,    -1,    27,    28,    29,
      30,    31,    32,   135,    -1,    -1,    36,    37,    38,    -1,
      -1,    -1,    -1,    -1,    -1,    45,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    53,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    63,    64,    -1,    -1,    -1,    68,    -1,
      -1,    71,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      80,    81,    82,    83,    84,    85,    -1,    -1,    -1,    -1,
     192,    -1,    -1,  1115,    -1,    95,    -1,    -1,    -1,    99,
      -1,   101,    -1,    -1,   104,   105,   106,   107,    -1,    -1,
     110,    -1,   112,    -1,    -1,   115,   116,    -1,    -1,    -1,
      -1,   121,    -1,    -1,    -1,    -1,    -1,   127,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   143,    -1,    -1,    -1,    -1,   148,    -1,
      -1,   151,    -1,   255,    -1,    -1,   156,    -1,    -1,    -1,
     160,    -1,    -1,   163,   164,   165,    -1,    -1,   168,    -1,
      -1,   171,    -1,   173,    -1,    -1,    -1,   279,   280,   281,
     282,   283,   284,   285,   286,   287,   288,   289,   290,   291,
     292,    -1,    -1,   295,   296,   297,    -1,    -1,   198,   199,
      -1,   201,   202,   203,   204,     1,    -1,    -1,    -1,     5,
      -1,     7,    -1,    -1,    -1,    -1,   318,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    27,    28,    29,    30,    -1,    -1,    -1,    -1,   239,
     240,   241,   242,    -1,    -1,    41,    -1,    -1,   248,   249,
      -1,    -1,   252,   253,   254,   255,   256,   257,   258,   259,
     260,   261,   262,   263,   264,   265,   266,   267,   268,   269,
     270,   271,   272,   273,   274,   275,    -1,    -1,   278,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   293,    -1,    91,    92,    93,   298,    95,
      -1,    -1,    98,    -1,    -1,   305,    -1,    -1,    -1,    -1,
      -1,   311,    -1,    -1,    -1,    -1,   316,    -1,    -1,   115,
      -1,    -1,   322,   323,    -1,   325,    -1,    -1,   328,   329,
     330,   331,    -1,    -1,    -1,   335,    -1,    -1,    -1,   339,
     340,   341,    -1,    -1,    -1,   345,   346,    -1,    -1,     1,
     350,    -1,    -1,     5,   354,     7,    -1,   357,   358,    -1,
      -1,    -1,    -1,   363,    -1,   365,   366,   367,    -1,   165,
     370,    -1,    24,    -1,    -1,    27,    28,    29,    30,    31,
      32,    -1,    -1,    -1,    36,    37,    38,    -1,    -1,    -1,
      -1,    -1,    -1,    45,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    53,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    63,    64,    -1,    -1,    -1,    68,    -1,    -1,    71,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    80,    81,
      82,    83,    84,    85,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    95,    -1,    -1,    -1,    99,    -1,   101,
      -1,    -1,   104,   105,   106,   107,    -1,    -1,   110,    -1,
     112,    -1,    -1,   115,   116,    -1,     1,    -1,    -1,   121,
       5,    -1,     7,    -1,    -1,   127,    -1,    -1,    -1,    -1,
      -1,    -1,   278,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   143,    27,    28,    29,    30,   148,   293,    -1,   151,
      -1,    -1,   298,    -1,   156,    -1,    41,    -1,   160,    -1,
      -1,   163,   164,   165,    -1,   311,   168,    -1,    -1,   171,
     316,   173,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   198,   199,    -1,   201,
     202,   203,   204,    -1,   350,    -1,    91,    92,    93,    -1,
      95,    -1,    -1,    98,    -1,    -1,    -1,    -1,    -1,    -1,
     366,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     115,    -1,    -1,    -1,    -1,    -1,    -1,   239,   240,   241,
     242,    -1,    -1,    -1,    -1,    -1,   248,   249,    -1,    -1,
     252,   253,   254,   255,   256,   257,   258,   259,   260,   261,
     262,   263,   264,   265,   266,   267,   268,   269,   270,   271,
     272,   273,   274,   275,    -1,    -1,   278,    -1,    -1,    -1,
     165,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   293,    -1,    -1,    -1,    -1,   298,    -1,    -1,    -1,
      -1,    -1,    -1,   305,    -1,    -1,    -1,    -1,    -1,   311,
      -1,    -1,    -1,    -1,   316,    -1,    -1,    -1,    -1,    -1,
     322,   323,    -1,   325,    -1,    -1,   328,   329,   330,   331,
      -1,    -1,    -1,   335,    -1,    -1,    -1,   339,   340,   341,
       5,    -1,     7,   345,   346,    -1,    -1,    -1,   350,    -1,
      -1,    -1,   354,    -1,    -1,   357,   358,    -1,    -1,    24,
      -1,   363,    -1,   365,   366,   367,    31,    32,   370,    -1,
      -1,    36,    37,    38,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    53,    -1,
      -1,    -1,    -1,   278,    -1,    -1,    -1,    -1,    63,    64,
      -1,    -1,   105,    68,    -1,    -1,    71,    -1,   293,    -1,
      -1,    -1,    -1,   298,    -1,    80,    81,    82,    83,    84,
      85,    -1,    -1,    -1,    -1,    -1,   311,    -1,    -1,   132,
      -1,   316,   135,    -1,    99,    -1,   101,    -1,    -1,   104,
     105,   106,   107,    -1,    -1,   110,    -1,   112,    -1,   152,
      -1,   116,    -1,    -1,    -1,    -1,   121,    -1,    -1,    -1,
      -1,    -1,   127,    -1,    -1,   350,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   143,    -1,
      -1,   366,    -1,   148,    -1,    -1,   151,    -1,    -1,    -1,
      -1,   156,    -1,    -1,    -1,   160,    -1,    -1,   163,   164,
      -1,    -1,    -1,   168,    -1,    -1,   171,    -1,   173,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   198,   199,    -1,   201,   202,   203,   204,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   255,   256,   257,   258,   259,   260,   261,   262,
     263,   264,   265,   266,   267,   268,   269,   270,   271,   272,
     273,   274,   275,    -1,   239,   240,   241,   242,    -1,    -1,
      -1,    -1,    -1,   248,   249,    -1,    -1,   252,   253,   254,
     255,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,    -1,    -1,   278,    -1,   318,    -1,     6,    -1,   322,
     323,    -1,   113,   114,    -1,    -1,   117,    -1,   293,   332,
      -1,    -1,    -1,   298,    -1,    24,    -1,    -1,    -1,    -1,
     305,    -1,    -1,    32,   135,    -1,   311,    -1,    37,    -1,
      -1,   316,    -1,    -1,    -1,   358,    -1,   322,   323,    -1,
     325,    -1,    -1,   328,   329,   330,   331,    -1,    -1,    -1,
     335,    -1,    -1,    -1,   339,   340,   341,    -1,    -1,    -1,
     345,   346,    71,    -1,    -1,    -1,    -1,    -1,    77,   354,
      -1,    -1,   357,   358,    -1,    -1,    -1,    86,   363,    88,
     365,   366,   367,    -1,    -1,   370,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   104,   105,   106,   107,    -1,
      -1,   110,    -1,   112,    -1,    -1,    -1,   116,    -1,    -1,
      -1,    -1,   121,    -1,    -1,    -1,    -1,    -1,   127,    -1,
      -1,    -1,   131,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   143,    -1,    -1,    -1,    -1,   148,
      -1,    -1,   151,    -1,   255,    -1,    -1,   156,    -1,    -1,
      -1,   160,    -1,    -1,   163,   164,    -1,    -1,    -1,   168,
      -1,    -1,   171,    -1,   173,    -1,    -1,    -1,   279,   280,
     281,   282,   283,   284,    -1,   286,   287,   288,   289,   290,
     291,   292,    -1,    -1,   295,   296,   297,    -1,    -1,   198,
     199,    91,   201,   202,   203,   204,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,     6,   318,    -1,    -1,
      -1,    -1,    -1,   113,   114,    -1,    -1,   117,    -1,    -1,
      -1,    -1,    -1,    -1,    24,    -1,    -1,    -1,    -1,    -1,
     239,    -1,    32,    -1,    -1,   135,    -1,    37,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   254,   255,   256,   257,   258,
     259,   260,   261,   262,   263,   264,   265,   266,   267,   268,
     269,   270,   271,   272,   273,   274,   275,    -1,    -1,   278,
      -1,    71,    -1,    -1,    -1,    -1,    -1,    77,    -1,   288,
      -1,    -1,   291,   292,    -1,    -1,    -1,    -1,    -1,   298,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   306,    -1,    -1,
      -1,    -1,    -1,    -1,   104,   105,   106,   107,    -1,    -1,
     110,    -1,   112,   322,   323,    -1,   116,    -1,    -1,   328,
     329,   121,    -1,    -1,    -1,    -1,    -1,   127,    -1,    -1,
      -1,   131,   341,    -1,    -1,    -1,   345,   346,    -1,    -1,
      -1,    -1,    -1,   143,    -1,    -1,   355,   356,   148,   358,
      -1,   151,    -1,    -1,    -1,   255,   156,    -1,   367,    -1,
     160,   370,    -1,   163,   164,    -1,    -1,    -1,   168,    -1,
      -1,   171,    -1,   173,    -1,    -1,    -1,    -1,    -1,   279,
     280,   281,   282,   283,   284,   285,   286,   287,   288,   289,
     290,   291,   292,    -1,    -1,   295,   296,   297,   198,   199,
      -1,   201,   202,   203,   204,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,     6,    -1,    -1,   318,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    24,    -1,    -1,    -1,    -1,    -1,   239,
      -1,    32,    -1,    -1,    -1,    -1,    37,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   254,   255,   256,   257,   258,   259,
     260,   261,   262,   263,   264,   265,   266,   267,   268,   269,
     270,   271,   272,   273,   274,   275,    -1,    -1,   278,    -1,
      71,    -1,    -1,    -1,    -1,    -1,    77,    -1,   288,    -1,
      -1,   291,   292,    -1,    -1,    -1,    -1,    -1,   298,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   104,   105,   106,   107,    -1,    -1,   110,
      -1,   112,   322,   323,    -1,   116,    -1,    -1,   328,   329,
     121,    -1,    -1,   110,    -1,   112,   127,    -1,    -1,   116,
     131,   341,    -1,    -1,   121,   345,   346,    -1,    -1,    -1,
     127,    -1,   143,    -1,    -1,   355,   356,   148,   358,    -1,
     151,    -1,    -1,    -1,    -1,   156,   143,   367,    -1,   160,
     370,   148,   163,   164,   151,    -1,    -1,   168,    -1,   156,
     171,    -1,   173,   160,    -1,    -1,   163,   164,   165,    -1,
      -1,   168,    -1,    -1,   171,    -1,   173,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   198,   199,    -1,
     201,   202,   203,   204,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   198,    -1,   200,     6,    -1,    -1,   204,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    24,    -1,    -1,    -1,    -1,    -1,   239,    -1,
      32,    -1,    -1,    -1,    -1,    37,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   254,   255,   256,   257,   258,   259,   260,
     261,   262,   263,   264,   265,   266,   267,   268,   269,   270,
     271,   272,   273,   274,   275,    -1,    -1,   278,    -1,    71,
      -1,    -1,    -1,    -1,    -1,    77,    -1,   288,    -1,    -1,
     291,   292,    -1,    -1,    -1,    -1,    -1,   298,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   104,   105,   106,   107,    -1,    -1,   110,    -1,
     112,   322,   323,    -1,   116,    -1,    -1,   328,   329,   121,
      -1,    -1,   110,    -1,   112,   127,    -1,   115,   116,   131,
     341,    -1,    -1,   121,   345,   346,    -1,    -1,    -1,   127,
      -1,   143,    -1,    -1,   355,   356,   148,   358,    -1,   151,
      -1,    -1,    -1,    -1,   156,   143,   367,    -1,   160,   370,
     148,   163,   164,   151,    -1,    -1,   168,    -1,   156,   171,
      -1,   173,   160,    -1,    -1,   163,   164,    -1,    -1,    -1,
     168,    -1,    -1,   171,    -1,   173,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   198,   199,    -1,   201,
     202,   203,   204,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     198,    -1,   200,     6,    -1,    -1,   204,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    24,    -1,    -1,    -1,    -1,    -1,   239,    -1,    32,
      -1,    -1,    -1,    -1,    37,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   254,   255,   256,   257,   258,   259,   260,   261,
     262,   263,   264,   265,   266,   267,   268,   269,   270,   271,
     272,   273,   274,   275,    -1,    -1,   278,    -1,    71,    -1,
      -1,    -1,    -1,    -1,    77,    -1,   288,    -1,    -1,   291,
     292,    -1,    -1,    -1,    -1,    -1,   298,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   104,   105,   106,   107,    -1,    -1,   110,    -1,   112,
     322,   323,    -1,   116,    -1,    -1,   328,   329,   121,    -1,
      -1,   110,    -1,   112,   127,    -1,    -1,   116,   131,   341,
      -1,    -1,   121,   345,   346,    -1,    -1,    -1,   127,    -1,
     143,    -1,    -1,   355,   356,   148,   358,    -1,   151,    -1,
      -1,    -1,    -1,   156,   143,   367,    -1,   160,   370,   148,
     163,   164,   151,    -1,    -1,   168,    -1,   156,   171,    -1,
     173,   160,    -1,    -1,   163,   164,    -1,    -1,    -1,   168,
      -1,    -1,   171,    -1,   173,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   198,   199,    -1,   201,   202,
     203,   204,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   198,
      -1,    -1,    -1,    -1,    -1,   204,    10,    11,    12,    13,
      14,    15,    -1,    -1,    -1,    -1,    20,    -1,    -1,    -1,
      24,    -1,    -1,    -1,    -1,    -1,   239,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   254,   255,   256,   257,   258,   259,   260,   261,   262,
     263,   264,   265,   266,   267,   268,   269,   270,   271,   272,
     273,   274,   275,    -1,    -1,   278,    -1,    71,    -1,    -1,
      -1,    -1,    -1,    77,    -1,   288,    -1,    -1,   291,   292,
      -1,    -1,    -1,    -1,    -1,   298,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     104,   105,   106,   107,    -1,    -1,    -1,    -1,   112,   322,
     323,    -1,   116,    -1,    -1,   328,   329,   121,    -1,    22,
      -1,    -1,    -1,   127,    -1,    -1,    -1,   131,   341,    -1,
      -1,    -1,   345,   346,    -1,    -1,    -1,    -1,    -1,   143,
      -1,    -1,   355,   356,   148,   358,    -1,   151,    -1,    -1,
      -1,    -1,   156,    -1,   367,    -1,   160,   370,    -1,   163,
     164,    -1,    -1,    -1,   168,    -1,    -1,   171,    71,   173,
      -1,    -1,    -1,    -1,    77,    -1,    -1,    -1,    -1,    -1,
      -1,   110,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   198,    -1,    -1,   201,   127,    -1,
     204,   104,   105,   106,   107,    -1,    -1,    -1,    -1,   112,
      -1,    -1,    -1,   116,   143,    -1,    -1,    -1,   121,   148,
      -1,    -1,   151,    -1,   127,    -1,    -1,   156,   131,    -1,
      -1,   160,    -1,    -1,   163,   164,    -1,    -1,    -1,   168,
     143,    -1,   171,    -1,   173,   148,    -1,    -1,   151,    -1,
      -1,    -1,    -1,   156,    -1,    -1,    -1,   160,    -1,    -1,
     163,   164,    -1,    -1,    -1,   168,    -1,    -1,   171,   198,
     173,    -1,    -1,    -1,    -1,   204,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   288,    -1,    -1,   291,   292,    -1,
      -1,    -1,    -1,    22,   298,   198,    -1,    -1,   201,    -1,
      -1,   204,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   328,   329,    -1,   331,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    71,    -1,    -1,    -1,    -1,    -1,    77,    -1,
      -1,   355,   356,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   365,    -1,   367,    -1,    -1,   370,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   104,   105,   106,   107,    -1,
      -1,    -1,    -1,   112,    -1,   288,    -1,   116,   291,   292,
      -1,    -1,   121,    -1,    -1,   298,    -1,    -1,   127,    -1,
      23,    24,   131,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   143,    -1,    -1,    -1,    -1,   148,
      -1,    -1,   151,    -1,    -1,   328,   329,   156,   331,    -1,
      -1,   160,    -1,    -1,   163,   164,    -1,    -1,    -1,   168,
      -1,    -1,   171,    -1,   173,    -1,    -1,    -1,    71,    -1,
      -1,    -1,   355,   356,    77,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   365,    -1,   367,    -1,    -1,   370,    -1,   198,
      -1,    -1,   201,    -1,    -1,   204,    -1,    -1,    -1,    -1,
      -1,   104,   105,   106,   107,    -1,    -1,    -1,    -1,   112,
      -1,    -1,    -1,   116,    -1,    -1,    -1,    -1,   121,    -1,
      -1,    -1,    -1,    -1,   127,    -1,    -1,    -1,   131,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     143,    -1,    -1,    -1,    -1,   148,    -1,    -1,   151,    -1,
      -1,    -1,    -1,   156,    -1,    -1,    -1,   160,    -1,    -1,
     163,   164,    -1,    -1,    -1,   168,    -1,    -1,   171,    -1,
     173,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   288,
      -1,    -1,   291,   292,    -1,    -1,    -1,    -1,    -1,   298,
      -1,    -1,    -1,    -1,    -1,   198,    -1,    -1,   201,    -1,
      -1,   204,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   328,
     329,    -1,   331,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   355,   356,    -1,    -1,
      -1,    -1,    24,    -1,    -1,    -1,    -1,    -1,   367,    31,
      32,   370,    -1,    -1,    36,    37,    38,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    53,    -1,    -1,    -1,   288,    -1,    -1,   291,   292,
      -1,    63,    64,    -1,    -1,   298,    68,    -1,    -1,    71,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    80,    81,
      82,    83,    84,    85,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   328,   329,    99,   331,   101,
      -1,    -1,   104,   105,   106,   107,    -1,    -1,   110,    -1,
     112,    -1,    -1,    -1,   116,    -1,    -1,    -1,    -1,   121,
      -1,    -1,   355,   356,    -1,   127,    -1,    -1,    -1,    -1,
      -1,    -1,   365,    -1,   367,    -1,    -1,   370,    -1,    -1,
      -1,   143,    -1,    -1,    -1,    -1,   148,    -1,    -1,   151,
      -1,    -1,    -1,    -1,   156,    -1,    -1,    -1,   160,    -1,
      -1,   163,   164,    -1,    -1,    -1,   168,    -1,    -1,   171,
      -1,   173,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   198,   199,    -1,   201,
     202,   203,   204,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   239,   240,   241,
     242,    -1,    -1,    -1,    -1,    -1,   248,   249,    -1,    -1,
     252,   253,   254,   255,   256,   257,   258,   259,   260,   261,
     262,   263,   264,   265,   266,   267,   268,   269,   270,   271,
     272,   273,   274,   275,    -1,    -1,   278,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    24,
      -1,   293,    -1,    -1,    -1,    -1,   298,    32,    -1,    -1,
      -1,    -1,    37,   305,    -1,    -1,    -1,    -1,    -1,   311,
      -1,    -1,    -1,    -1,   316,    -1,    -1,    -1,    -1,    -1,
     322,   323,    -1,   325,    -1,    -1,   328,   329,   330,   331,
      -1,    -1,    -1,   335,    -1,    -1,    71,   339,   340,   341,
      -1,    -1,    77,   345,   346,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   354,    -1,    -1,   357,   358,    -1,    -1,    -1,
      -1,   363,    -1,   365,    -1,   367,    -1,    -1,   370,   104,
     105,   106,   107,    -1,    -1,   110,    -1,   112,    -1,    -1,
      -1,   116,    -1,    -1,    -1,    -1,   121,    -1,    -1,    -1,
      -1,    -1,   127,    -1,    -1,    -1,   131,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   143,    -1,
      -1,    -1,    -1,   148,    -1,    -1,   151,    -1,    -1,    -1,
      -1,   156,    -1,    -1,    -1,   160,    -1,    -1,   163,   164,
      -1,    -1,    -1,   168,    -1,    -1,   171,    -1,   173,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   198,   199,    -1,   201,   202,   203,   204,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    24,    -1,
      -1,    -1,    -1,    -1,   239,    31,    32,    -1,    -1,    -1,
      -1,    37,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   254,
     255,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,    -1,    -1,   278,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   288,    -1,    -1,   291,   292,    -1,    -1,
      -1,    -1,    -1,   298,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   104,   105,
     106,   107,    -1,    -1,   110,    -1,   112,   322,   323,    -1,
     116,    -1,    -1,   328,   329,   121,    -1,    -1,    -1,    -1,
      -1,   127,    -1,    -1,    -1,    -1,   341,    -1,    -1,    -1,
     345,   346,    -1,    -1,    -1,    -1,    -1,   143,    -1,    -1,
     355,   356,   148,   358,    -1,   151,    -1,    -1,    -1,    -1,
     156,    -1,   367,    -1,   160,   370,    -1,   163,   164,    -1,
      -1,    -1,   168,    -1,    -1,   171,    -1,   173,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   198,   199,    24,   201,   202,   203,   204,    -1,
      -1,    31,    32,    -1,    -1,    -1,    -1,    37,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   239,   240,   241,   242,    -1,    -1,    -1,
      -1,    -1,   248,   249,    -1,    -1,   252,   253,   254,   255,
     256,   257,   258,   259,   260,   261,   262,   263,   264,   265,
     266,   267,   268,   269,   270,   271,   272,   273,   274,   275,
      -1,    -1,   278,    -1,   104,   105,   106,   107,    -1,    -1,
     110,    -1,   112,    -1,    -1,    -1,   116,    -1,    -1,    -1,
      -1,   121,    -1,    -1,    -1,    -1,    -1,   127,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   143,    -1,    -1,   322,   323,   148,    -1,
      -1,   151,    -1,    -1,    -1,    -1,   156,    -1,    -1,    -1,
     160,    -1,    -1,   163,   164,   341,    -1,    -1,   168,   345,
     346,   171,    -1,   173,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   357,   358,    -1,    -1,    -1,    -1,    -1,    -1,   365,
      -1,   367,    -1,    -1,   370,    -1,    -1,    -1,   198,   199,
      -1,   201,   202,   203,   204,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    24,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   239,
     240,   241,   242,    -1,    -1,    -1,    -1,    -1,   248,   249,
      -1,    -1,   252,   253,   254,   255,   256,   257,   258,   259,
     260,   261,   262,   263,   264,   265,   266,   267,   268,   269,
     270,   271,   272,   273,   274,   275,    71,    -1,   278,    -1,
      -1,    -1,    77,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   104,
     105,   106,   107,    -1,    -1,    -1,    -1,   112,    -1,    -1,
      -1,   116,   322,   323,    -1,    -1,   121,    -1,    -1,    -1,
      -1,    -1,   127,    -1,    -1,    -1,   131,    -1,    -1,    -1,
      -1,   341,    -1,    -1,    -1,   345,   346,    -1,   143,    -1,
      -1,    -1,    -1,   148,    -1,    -1,   151,   357,   358,    -1,
      -1,   156,    -1,    -1,    -1,   160,    -1,   367,   163,   164,
     370,    -1,    -1,   168,    -1,    24,   171,    -1,   173,    -1,
      -1,    -1,    31,    32,    -1,    -1,    -1,    -1,    37,    38,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   198,    -1,    -1,   201,    -1,    -1,   204,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    88,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   105,    -1,    -1,    -1,
      -1,   110,    -1,   112,    -1,    -1,    -1,   116,    -1,    -1,
      -1,    -1,   121,    -1,    -1,    -1,    -1,    -1,   127,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   288,   143,    -1,   291,   292,    -1,   148,
      -1,    -1,   151,   298,    -1,    -1,    -1,   156,    -1,    -1,
      -1,   160,    -1,    -1,   163,   164,    -1,    -1,    -1,   168,
      -1,    -1,   171,    -1,   173,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   328,   329,    -1,   331,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   198,
     199,    24,   201,   202,   203,   204,    -1,    -1,    31,    32,
     355,   356,    -1,    -1,    37,    -1,    -1,    -1,    -1,    -1,
     365,    -1,   367,    -1,    -1,   370,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     239,   240,   241,   242,    -1,    -1,    -1,    -1,    -1,   248,
     249,    -1,    -1,   252,   253,   254,   255,   256,   257,   258,
     259,   260,   261,   262,   263,   264,   265,   266,   267,   268,
     269,   270,   271,   272,   273,   274,   275,    -1,    -1,   278,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   110,    -1,   112,
      -1,    -1,    -1,   116,   293,    -1,    -1,    -1,   121,    -1,
      -1,    -1,    -1,    -1,   127,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   311,    -1,    -1,    -1,    -1,   316,    -1,   318,
     143,    -1,    -1,   322,   323,   148,    -1,    -1,   151,    -1,
      -1,    -1,    -1,   156,    -1,    -1,   335,   160,    -1,    -1,
     163,   164,   341,    -1,    -1,   168,   345,   346,   171,    -1,
     173,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   357,   358,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   198,   199,    -1,   201,   202,
     203,   204,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   239,   240,   241,   242,
      -1,    -1,    -1,    -1,    -1,   248,   249,    -1,    -1,   252,
     253,   254,   255,   256,   257,   258,   259,   260,   261,   262,
     263,   264,   265,   266,   267,   268,   269,   270,   271,   272,
     273,   274,   275,    -1,    -1,   278,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,     0,     1,    -1,    -1,    -1,     5,    -1,
       7,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   322,
     323,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      27,    28,    29,    30,    -1,    -1,    -1,    -1,   341,    -1,
      -1,    38,   345,   346,    -1,    -1,    -1,    -1,    -1,    46,
      -1,    48,    -1,    50,   357,   358,    53,    54,    55,    -1,
      57,    -1,    -1,    -1,    61,    62,    63,    -1,    65,    66,
      67,    68,    69,    70,    71,    72,    73,    74,    75,    76,
      77,    78,    -1,    -1,    -1,    -1,    71,    -1,    -1,    -1,
      -1,    -1,    77,    -1,    -1,    80,    81,    82,    83,    84,
      85,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   105,    -1,
      -1,    -1,    -1,    -1,   111,    -1,    -1,    -1,    -1,   104,
     105,   106,   107,    -1,   121,    -1,    -1,   112,    -1,    -1,
     127,   116,    -1,    -1,    -1,    -1,   121,    -1,    -1,    -1,
      -1,    -1,   127,    -1,    -1,    -1,   131,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   151,    -1,    -1,    -1,   143,    -1,
      -1,    -1,    -1,   148,    -1,    -1,   151,    71,    -1,    -1,
      -1,   156,    -1,    77,    -1,   160,    -1,    -1,   163,   164,
      -1,    -1,    -1,   168,    -1,    -1,   171,    -1,   173,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     104,   105,   106,   107,    -1,    -1,    -1,    -1,   112,    -1,
      -1,    -1,   116,   198,    -1,    -1,   201,   121,    -1,   204,
      -1,    -1,    -1,   127,    -1,    -1,    -1,   131,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   143,
      -1,    -1,    -1,    -1,   148,    -1,    -1,   151,    -1,    -1,
      -1,    -1,   156,    -1,    -1,    -1,   160,    -1,    -1,   163,
     164,    -1,    -1,    -1,   168,    -1,    -1,   171,    -1,   173,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   278,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   198,    -1,   293,   201,    -1,    -1,
     204,   298,    -1,   288,    -1,    -1,   291,   292,    -1,    -1,
      -1,    -1,    -1,   298,   311,    -1,    -1,    -1,    -1,   316,
      -1,   318,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   328,   329,    -1,   331,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    71,    -1,    -1,    -1,
      -1,    -1,    77,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     355,   356,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   367,    -1,   288,   370,    -1,   291,   292,   104,
     105,   106,   107,    -1,   298,    -1,    -1,   112,    -1,    -1,
      -1,   116,    -1,    -1,    -1,    -1,   121,    -1,    -1,    -1,
      -1,    -1,   127,    -1,    -1,    -1,   131,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   328,   329,    -1,   331,   143,    -1,
      -1,    -1,    -1,   148,    -1,    -1,   151,    -1,    -1,    -1,
      -1,   156,    -1,    -1,    -1,   160,    -1,    -1,   163,   164,
      -1,   355,   356,   168,    -1,    -1,   171,    -1,   173,    -1,
      -1,    -1,    -1,   367,    -1,    -1,   370,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   198,    -1,    -1,   201,    -1,    -1,   204,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   288,    -1,    -1,   291,   292,    -1,    -1,
      -1,    -1,    -1,   298,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   328,   329,    -1,   331,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     355,   356,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   367,    -1,    -1,   370
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int16 yystos[] =
{
       0,   374,     0,     1,     5,     7,    27,    28,    29,    30,
      38,    46,    48,    50,    53,    54,    55,    57,    61,    62,
      63,    65,    66,    67,    68,    69,    70,    71,    72,    73,
      74,    75,    76,    77,    78,   111,   121,   127,   151,   278,
     293,   311,   316,   318,   375,   431,   432,   433,   434,   518,
     519,   520,   522,   537,   375,   106,   105,   298,   514,   514,
     514,   520,   531,   520,   522,   537,   520,   525,   525,   525,
     520,   528,   434,    50,   435,    38,    46,    48,    53,    54,
      55,    57,   278,   293,   311,   316,   318,   436,    50,   437,
      38,    46,    48,    50,    53,    54,    55,    57,   278,   293,
     311,   316,   318,   442,    54,   444,    38,    43,    46,    47,
      48,    49,    52,    53,    55,    56,    57,    59,    99,   101,
     102,   103,   278,   293,   300,   301,   302,   303,   311,   316,
     318,   319,   447,    50,    51,    53,    54,    55,   293,   300,
     301,   316,   450,    46,    48,    53,    55,    59,    99,   101,
     451,    48,   452,    24,    31,    32,    37,   104,   105,   106,
     107,   110,   112,   116,   121,   127,   143,   148,   151,   156,
     160,   163,   164,   168,   171,   173,   198,   199,   201,   202,
     203,   204,   239,   240,   241,   242,   248,   249,   252,   253,
     254,   255,   256,   257,   258,   259,   260,   261,   262,   263,
     264,   265,   266,   267,   268,   269,   270,   271,   272,   273,
     274,   275,   278,   322,   323,   341,   345,   346,   357,   358,
     367,   370,   460,   515,   651,   652,   655,   656,   657,   658,
     662,   726,   729,   731,   735,   740,   741,   743,   745,   755,
     757,   759,   761,   763,   765,   769,   771,   773,   775,   777,
     779,   781,   783,   785,   787,   791,   793,   795,   797,   808,
     816,   818,   820,   821,   823,   825,   827,   829,   831,   833,
     835,   837,    59,   351,   352,   353,   453,   459,    59,   454,
     459,    38,    46,    48,    50,    53,    54,    55,    57,   278,
     293,   311,   316,   318,   443,   105,   455,   456,   378,   397,
     398,    91,   285,   287,   531,   531,   531,   531,     0,   375,
     514,   514,    58,   348,   349,   534,   535,   536,    36,    38,
      53,    63,    64,    68,    71,    80,    81,    82,    83,    84,
      85,    99,   101,   254,   278,   293,   298,   305,   311,   316,
     325,   328,   329,   330,   331,   335,   339,   340,   354,   363,
     365,   541,   542,   543,   545,   546,   547,   548,   549,   550,
     551,   552,   553,   554,   555,   556,   557,   561,   562,   564,
     565,   568,   569,   571,   572,   579,   580,   585,   586,   595,
     596,   599,   600,   601,   602,   603,   624,   625,   627,   628,
     630,   631,   634,   635,   636,   646,   647,   648,   649,   650,
     657,   664,   665,   666,   667,   668,   669,   673,   674,   675,
     710,   724,   729,   730,   753,   754,   755,   798,   375,   364,
     364,   375,   514,   607,   461,   466,   541,   514,   473,   476,
     651,   675,   479,   514,   487,   522,   538,   531,   520,   522,
     525,   525,   525,   528,    91,   285,   287,   531,   531,   531,
     531,   537,   441,   520,   531,   532,   438,   518,   520,   521,
     439,   520,   522,   523,   538,   440,   520,   525,   526,   525,
     525,   520,   528,   529,    91,   285,   287,   699,   441,   441,
     441,   441,   525,   531,   449,   519,   540,   520,   540,   522,
     540,    46,   445,   446,   519,   520,   540,   525,   525,   446,
     528,   540,    46,    47,   525,   540,   446,    91,   285,   304,
     699,   700,   531,   446,   446,   446,   446,   531,   531,   531,
     446,   404,   538,    48,   445,   520,   522,   540,   440,   525,
     440,   531,   446,   446,   531,   416,   520,   522,   525,   525,
     540,    46,   525,   522,   105,   108,   109,   110,   758,   113,
     114,   255,   256,   259,   660,   661,    33,    34,    35,   255,
     732,   134,   663,   169,   170,   819,   113,   114,   115,   760,
     115,   117,   118,   119,   120,   762,   113,   114,   122,   123,
     124,   125,   126,   764,   113,   114,   117,   128,   129,   130,
     131,   132,   133,   134,   135,   136,   178,   766,   115,   117,
     136,   144,   145,   146,   147,   770,   115,   136,   149,   307,
     772,   113,   114,   128,   130,   131,   132,   153,   154,   155,
     774,   114,   115,   117,   136,   144,   145,   147,   157,   158,
     159,   776,   129,   145,   154,   161,   162,   778,   145,   162,
     780,   154,   165,   166,   782,   132,   136,   169,   170,   784,
     136,   169,   170,   172,   786,   136,   145,   161,   165,   169,
     170,   174,   175,   176,   177,   178,   799,   115,   169,   170,
     178,   809,   165,   200,   759,   761,   763,   765,   769,   771,
     773,   775,   777,   779,   781,   783,   785,   787,   788,   789,
     790,   792,   808,   816,   818,   128,   135,   165,   394,   796,
     394,   115,   200,   790,   794,   136,   169,   170,   205,   238,
     817,   115,   127,   129,   147,   151,   154,   243,   276,   277,
     358,   742,   744,   824,   244,   826,   244,   828,   165,   245,
     246,   247,   830,   129,   154,   822,   117,   133,   154,   161,
     250,   251,   832,   129,   154,   834,   115,   129,   136,   154,
     161,   836,   105,   132,   135,   152,   318,   332,   358,   727,
     728,   729,   113,   114,   117,   135,   255,   279,   280,   281,
     282,   283,   284,   286,   287,   288,   289,   290,   291,   292,
     295,   296,   297,   318,   746,   747,   750,   332,   342,   734,
     669,   674,   343,   239,   248,   249,   252,   253,   838,   361,
     362,   403,   737,   668,   514,   422,   459,   352,   402,   459,
     390,   441,   438,   439,   522,   538,   440,   525,   525,   528,
     529,   699,   441,   441,   441,   441,   385,   408,    47,    49,
      51,    52,    59,    60,    93,   457,   531,   531,   531,   382,
     694,   709,   696,   698,   104,   104,   104,    86,   742,   294,
     647,   173,   514,   651,   725,   725,    63,   100,   514,   105,
     727,    91,   192,   285,   746,   747,   294,   294,   312,   294,
      86,   165,    86,    86,   742,   105,     5,   376,   676,   677,
     350,   539,   552,   430,   466,   104,   314,   563,   381,   382,
     295,   296,   566,   567,   317,   570,   383,   428,   165,   306,
     307,   308,   309,   310,   573,   574,   314,   315,   581,   588,
     405,   314,   315,   582,   587,   413,   415,   326,   598,   409,
       6,    71,    77,    86,    88,   112,   116,   121,   127,   131,
     151,   239,   288,   291,   292,   298,   306,   328,   329,   355,
     356,   367,   610,   611,   612,   613,   614,   615,   616,   618,
     619,   620,   621,   622,   623,   652,   655,   662,   719,   720,
     721,   726,   731,   735,   741,   742,   743,   745,   751,   752,
     755,   423,   429,    39,    40,   188,   191,   604,   605,   409,
      86,   332,   333,   334,   626,   632,   633,   409,    86,   629,
     632,   387,   393,   414,   336,   337,   338,   637,   638,   642,
     643,    24,   651,   653,   654,    16,    17,    18,    19,   369,
       9,    25,    55,    10,    11,    12,    13,    14,    15,    20,
     112,   116,   121,   127,   143,   148,   151,   156,   160,   163,
     164,   168,   171,   173,   198,   201,   204,   331,   367,   652,
     654,   655,   670,   671,   672,   675,   711,   712,   713,   714,
     715,   716,   717,   718,   720,   721,   722,   723,    53,    53,
      23,   365,   692,   711,   712,   717,   692,    39,   365,   606,
     365,   365,   365,   365,   365,   534,   541,   607,   461,   466,
     473,   476,   479,   487,   531,   531,   531,   382,   694,   709,
     696,   698,   541,   429,    58,    58,    58,   466,    58,   476,
      58,   487,   531,   382,   405,   413,   420,   476,   429,    44,
     448,   520,   525,   540,   531,    46,   382,   405,   413,   420,
     522,   476,   382,   413,   525,   514,   425,     8,     9,   115,
     259,   260,   659,   310,   421,   105,   128,   294,   425,   424,
     389,   424,   399,   112,   127,   112,   127,   378,   139,   140,
     141,   142,   767,   397,   424,   400,   424,   401,   398,   424,
     400,   377,   388,   380,   426,   427,    24,    39,   104,   176,
     179,   180,   181,   182,   183,   184,   185,   186,   187,   188,
     189,   190,   191,   800,   801,   802,   424,   104,   384,   422,
     788,   394,   790,   183,   206,   207,   208,   209,   210,   211,
     212,   213,   214,   215,   216,   217,   218,   219,   220,   221,
     222,   810,   815,   418,   424,   397,   398,   403,   744,   417,
     417,   371,   417,   417,   371,   417,   396,   392,   386,   424,
     407,   406,   420,   406,   420,   113,   114,   127,   135,   151,
     279,   280,   281,   748,   749,   750,   381,   343,   343,   104,
     417,   396,   392,   386,   407,   360,   736,   368,   429,   476,
     487,   531,   382,   405,   413,   420,   458,   459,   707,   707,
     707,   295,   365,   693,   312,   365,   708,   365,   582,   695,
     365,   515,   697,     6,   127,   151,   622,    86,   622,   644,
     645,   669,   178,    24,    24,    98,   365,    53,    53,    53,
      53,   750,    53,   622,   622,   622,   639,   640,   641,   651,
     655,   669,   673,   735,   741,   642,   622,   622,    86,   515,
      22,   675,   680,   681,   682,   690,   717,   718,     8,   366,
     515,   365,   104,   104,   104,   567,   105,   299,   516,    79,
     112,   127,   173,   262,   576,   515,   104,   104,   104,   515,
     575,   574,   104,   590,   592,   593,   104,   143,   156,   173,
     327,   622,   416,   385,     6,   622,    86,   389,   399,   378,
     397,   398,   383,    86,   409,   409,   615,   652,   721,    16,
      17,    18,    19,   369,    21,    23,     9,    55,     6,   632,
      86,    88,   244,   306,     8,     8,   104,   104,   605,     6,
       8,     6,   622,   640,   651,   655,   638,     8,   514,   365,
     722,   722,   713,   714,   715,   668,   365,   558,   653,   712,
     397,   400,   398,   400,   377,   388,   380,   426,   427,   422,
     384,   394,   418,   409,   717,     8,    21,    16,    17,    18,
      19,   369,     8,    21,    23,     9,   711,   712,   717,   622,
     622,   104,   366,   375,    21,   375,   104,   502,   429,   465,
     467,   475,   484,   488,   606,   365,   365,   365,   365,   365,
     707,   707,   707,   693,   708,   695,   697,   104,   104,   104,
     365,   104,   104,   365,   707,   105,   381,   520,   104,   661,
     424,   391,   104,   411,   411,   389,   397,   389,   397,   115,
     132,   137,   138,   244,   397,   768,   379,    98,   806,   192,
     804,   197,   807,   195,   196,   805,   193,   194,   803,   132,
     384,   228,   232,   233,   234,   814,   223,   224,   225,   226,
     812,   227,   228,   229,   230,   231,   813,   813,   232,   235,
     235,   236,   237,   236,   115,   132,   165,   811,   419,   417,
     104,   104,   113,   114,   113,   114,   381,   381,   104,   104,
     344,   733,   104,   162,   359,   738,   742,   365,   707,   365,
     365,   365,   104,   495,   382,   588,   500,   405,   496,   104,
     413,   501,   420,   622,     6,     6,   622,   429,   653,    91,
      94,   539,   683,   685,    39,   176,   181,   191,   801,   802,
     515,   515,   104,   669,   678,   679,   622,   622,   622,   622,
      53,   622,   382,   405,   413,    23,   414,    86,   336,    44,
     622,   376,     6,   376,   278,   293,   311,   316,   562,   564,
     569,   580,   586,   687,   688,    91,    94,   539,   686,   689,
     376,   677,   468,   389,   150,   145,   150,   577,   578,    21,
     105,   296,   313,   589,   313,   591,    21,   105,   296,   583,
     105,   115,   597,   655,   115,   597,   422,   115,   597,   622,
       6,   622,   622,   368,   610,   610,   611,   612,   613,   104,
     615,   610,   617,   653,   675,   622,   622,    86,     9,    86,
     652,   721,   751,   751,   622,   633,   622,   632,   643,   379,
     644,   376,   559,   560,   368,   717,   711,   717,   722,   722,
     713,   714,   715,   717,   104,   711,   717,   672,   717,    21,
      21,   104,    40,   375,   366,   375,   431,   539,   606,    38,
      48,    53,    55,    57,   165,   278,   293,   311,   316,   318,
     366,   375,   431,   462,   539,    45,    95,   115,   165,   366,
     375,   431,   504,   510,   511,   539,   541,    41,    90,    91,
      92,    93,    95,    98,   115,   165,   366,   375,   431,   474,
     485,   539,   544,   551,    41,    91,    92,    93,   115,   165,
     366,   375,   431,   485,   539,   544,    42,    45,   165,   293,
     366,   375,   431,   429,   465,   467,   475,   484,   488,   365,
     365,   365,   382,   405,   413,   420,   467,   488,   381,   381,
       8,   421,   424,   397,   802,   424,   418,   372,   372,   397,
     397,   398,   398,   733,   347,   733,   104,   395,   403,   113,
     114,   739,   488,   381,   498,   499,   497,   296,   366,   375,
     431,   539,   693,   590,   592,   366,   375,   431,   539,   708,
     366,   375,   431,   539,   695,   583,   366,   375,   431,   539,
     697,   622,   622,     6,   516,   516,   685,   422,   379,   379,
     365,   552,   683,   406,   406,   381,   381,   622,   381,   639,
     651,   655,   641,   640,   622,    44,    83,    84,   691,   718,
     724,   381,   382,   383,   405,   413,   688,   105,   684,   516,
     687,   689,   366,   375,   541,   397,     8,   422,   320,   321,
     322,   323,   324,   594,   104,   104,   593,   594,   317,   584,
     597,   597,    71,   597,   622,     6,   622,   167,   622,   632,
     632,     6,   366,   544,   680,     8,   366,   711,   711,   104,
      40,   429,   514,   533,   514,   524,   514,   527,   527,   514,
     530,   105,   463,   464,    91,   285,   287,   533,   533,   533,
     533,   375,   364,    80,    81,   512,   513,   651,   424,   100,
     375,   375,   375,   375,   375,   471,   472,   656,   516,   516,
     364,    96,    97,   486,   104,   105,   130,   131,   255,   275,
     276,   492,   493,   503,    87,    88,    89,    91,   477,   478,
     375,   375,   375,   375,   551,   472,   516,   516,   364,   493,
     477,   375,   375,   375,   105,   364,   100,   382,   366,   366,
     366,   366,   366,   498,   499,   497,   366,   366,   104,   756,
       8,   410,   104,   395,   403,   366,    95,   135,   279,   366,
     375,   431,   539,   705,    91,    98,   135,   170,   279,   366,
     375,   431,   539,   706,   115,   279,   366,   375,   431,   539,
     702,   104,   382,   589,   591,   405,   413,   584,   420,   622,
     678,   366,   381,   414,   414,   622,   376,   375,   578,   295,
     105,   104,   424,   424,   416,   424,   622,    86,   644,     6,
     366,     6,   376,   560,   191,   608,   104,   494,   466,   473,
     479,   487,     8,   533,   533,   533,   494,   494,   494,   494,
     365,   489,   651,   412,   105,     9,   375,   375,   476,   412,
       9,   424,     8,   375,     6,   375,   375,   476,     6,   375,
     152,   505,   489,   375,   366,   366,   366,   379,   104,   733,
     364,   168,   173,   701,   519,   381,   516,   104,   701,   104,
     519,   381,   106,   519,   381,   552,   583,   400,   400,   622,
     366,   644,   724,   188,   609,   375,   365,   365,   365,   365,
     365,   464,   494,   494,   494,   365,   365,   365,   365,   105,
     106,   490,   491,   651,   375,    42,   471,   492,   478,    38,
      88,   105,   278,   293,   311,   316,   318,   335,   469,   470,
     472,   481,   482,   656,    88,   481,   483,    23,   104,   105,
     362,   506,   507,   508,   651,   375,   375,   381,   381,   381,
       8,   395,   365,   426,   422,   375,   375,   375,   375,   375,
     375,   375,   135,   375,   366,   366,   376,   608,   502,   467,
     475,   484,   488,   365,   365,   365,   495,   500,   496,   501,
       8,   366,   105,   429,   472,    91,   285,   480,   382,   405,
     413,   420,   375,     9,   375,   375,   493,   104,    23,    26,
     756,   105,   703,   704,   701,   609,   366,   366,   366,   366,
     366,   498,   499,   497,   366,   366,   366,   366,   376,   491,
      44,    45,   509,   381,   656,   424,   375,   104,   104,     6,
       8,   366,   375,   375,   375,   375,   375,   375,   366,   366,
     366,   375,   375,   375,   375,   515,   651,   364,   505,   424,
     104,   516,   517,   704,   375,   424,   429,   375,   375,   375,
     382,   405,   413,   420,   489,   412,   381,   381,   381
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int16 yyr1[] =
{
       0,   373,   374,   374,   375,   375,   376,   376,   377,   378,
     379,   380,   381,   382,   383,   384,   385,   386,   387,   388,
     389,   390,   391,   392,   393,   394,   395,   396,   397,   398,
     399,   400,   401,   402,   403,   404,   405,   406,   407,   408,
     409,   410,   411,   412,   413,   414,   415,   416,   417,   418,
     419,   420,   421,   422,   423,   424,   425,   426,   427,   428,
     429,   430,   431,   431,   431,   431,   431,   432,   432,   432,
     432,   433,   433,   433,   433,   433,   433,   433,   433,   433,
     433,   433,   433,   433,   433,   433,   433,   434,   434,   434,
     434,   434,   434,   434,   434,   434,   434,   434,   434,   434,
     434,   434,   434,   434,   434,   434,   434,   434,   434,   434,
     434,   435,   436,   436,   436,   436,   436,   436,   436,   436,
     436,   436,   436,   436,   436,   436,   436,   436,   436,   437,
     438,   438,   439,   439,   440,   440,   441,   441,   442,   442,
     442,   442,   442,   442,   442,   442,   442,   442,   442,   442,
     442,   442,   442,   442,   443,   443,   443,   443,   443,   443,
     443,   443,   443,   443,   443,   443,   443,   443,   443,   444,
     445,   445,   446,   446,   447,   447,   447,   447,   447,   447,
     447,   447,   447,   447,   447,   447,   447,   447,   447,   447,
     447,   447,   447,   447,   447,   447,   447,   447,   447,   447,
     447,   447,   448,   449,   449,   450,   450,   450,   450,   450,
     450,   450,   450,   450,   450,   450,   450,   451,   451,   451,
     451,   451,   451,   451,   452,   453,   453,   454,   454,   455,
     456,   456,   457,   457,   457,   457,   457,   457,   457,   457,
     458,   458,   459,   459,   459,   460,   461,   462,   462,   463,
     463,   464,   465,   465,   465,   465,   465,   465,   465,   465,
     465,   465,   465,   465,   465,   465,   465,   465,   466,   467,
     467,   467,   467,   467,   467,   467,   467,   467,   468,   468,
     468,   469,   469,   470,   470,   470,   470,   471,   472,   472,
     473,   474,   474,   475,   475,   475,   475,   475,   475,   475,
     475,   475,   475,   475,   475,   476,   476,   477,   477,   478,
     478,   478,   478,   479,   480,   480,   481,   481,   481,   481,
     481,   482,   482,   483,   483,   484,   484,   484,   484,   484,
     484,   484,   484,   484,   484,   484,   484,   484,   484,   485,
     485,   486,   486,   487,   488,   488,   488,   488,   488,   488,
     488,   489,   489,   490,   490,   490,   491,   491,   491,   492,
     492,   493,   493,   494,   495,   495,   495,   495,   495,   496,
     496,   496,   496,   496,   497,   497,   497,   497,   497,   498,
     498,   498,   498,   498,   499,   499,   499,   499,   499,   500,
     500,   500,   500,   500,   501,   501,   501,   501,   501,   502,
     502,   502,   502,   502,   503,   503,   503,   503,   503,   504,
     505,   506,   506,   507,   507,   507,   507,   507,   508,   508,
     509,   509,   509,   509,   510,   511,   512,   512,   513,   513,
     514,   514,   515,   515,   515,   516,   517,   517,   518,   518,
     519,   519,   519,   519,   519,   519,   520,   521,   522,   523,
     524,   525,   526,   527,   528,   529,   530,   531,   532,   533,
     534,   535,   536,   537,   537,   537,   537,   538,   539,   540,
     540,   541,   541,   542,   543,   543,   544,   544,   545,   546,
     547,   548,   549,   549,   550,   550,   550,   550,   550,   551,
     551,   551,   551,   551,   552,   552,   552,   552,   552,   552,
     552,   552,   552,   552,   552,   552,   552,   552,   552,   552,
     552,   552,   552,   552,   552,   552,   552,   553,   554,   554,
     555,   556,   556,   557,   558,   558,   559,   559,   559,   560,
     561,   562,   563,   563,   564,   564,   565,   566,   566,   567,
     567,   568,   569,   569,   570,   570,   571,   571,   572,   573,
     573,   574,   574,   574,   574,   574,   574,   575,   576,   576,
     576,   576,   576,   577,   577,   578,   578,   579,   580,   581,
     581,   582,   582,   582,   583,   583,   584,   584,   585,   586,
     587,   588,   588,   588,   589,   589,   590,   591,   591,   592,
     592,   593,   593,   594,   594,   594,   594,   594,   595,   596,
     597,   597,   598,   598,   598,   598,   598,   598,   598,   598,
     599,   600,   600,   601,   601,   601,   601,   601,   601,   602,
     602,   603,   604,   604,   605,   605,   605,   605,   606,   606,
     607,   608,   608,   609,   609,   610,   610,   610,   610,   610,
     610,   610,   610,   610,   610,   610,   610,   610,   611,   611,
     611,   612,   612,   613,   613,   614,   614,   615,   616,   616,
     617,   617,   618,   618,   619,   620,   621,   621,   622,   622,
     622,   623,   623,   623,   623,   623,   623,   623,   623,   623,
     623,   623,   623,   623,   623,   624,   624,   625,   626,   626,
     626,   627,   627,   628,   629,   629,   629,   629,   629,   630,
     630,   631,   631,   632,   632,   633,   633,   633,   634,   634,
     634,   634,   635,   635,   636,   637,   637,   638,   638,   639,
     639,   640,   640,   640,   641,   641,   641,   641,   642,   642,
     643,   643,   644,   644,   645,   646,   646,   646,   647,   647,
     647,   648,   648,   649,   649,   650,   651,   652,   652,   653,
     653,   654,   655,   656,   656,   656,   656,   656,   656,   656,
     656,   656,   656,   656,   656,   657,   657,   657,   657,   658,
     659,   659,   659,   660,   660,   660,   660,   660,   661,   661,
     662,   662,   663,   663,   664,   664,   664,   665,   665,   666,
     666,   667,   667,   668,   669,   669,   670,   671,   672,   672,
     673,   674,   674,   674,   675,   676,   676,   676,   677,   677,
     677,   678,   678,   679,   680,   680,   680,   681,   681,   682,
     682,   683,   683,   684,   685,   685,   685,   686,   686,   687,
     687,   688,   688,   688,   688,   688,   689,   689,   689,   690,
     691,   691,   692,   692,   692,   692,   693,   694,   695,   696,
     697,   698,   699,   699,   699,   700,   700,   700,   701,   701,
     702,   702,   703,   703,   704,   705,   705,   705,   706,   706,
     706,   706,   706,   707,   708,   708,   709,   710,   710,   710,
     710,   710,   710,   710,   710,   711,   711,   712,   712,   712,
     713,   713,   713,   714,   714,   715,   715,   716,   716,   717,
     718,   718,   718,   718,   719,   719,   720,   721,   721,   721,
     721,   721,   721,   721,   721,   721,   721,   721,   721,   721,
     721,   722,   722,   722,   722,   722,   722,   722,   722,   722,
     722,   722,   722,   722,   722,   722,   722,   722,   722,   723,
     723,   723,   723,   723,   723,   723,   724,   724,   724,   724,
     724,   724,   725,   725,   726,   726,   726,   727,   727,   728,
     728,   728,   728,   728,   729,   729,   729,   729,   729,   729,
     729,   729,   729,   729,   729,   729,   729,   729,   729,   729,
     729,   729,   729,   729,   729,   729,   729,   729,   730,   730,
     730,   730,   730,   730,   731,   731,   732,   732,   732,   733,
     733,   734,   734,   735,   736,   736,   737,   737,   738,   738,
     739,   739,   740,   740,   741,   741,   741,   742,   742,   743,
     743,   744,   744,   744,   744,   745,   745,   745,   746,   746,
     747,   747,   747,   747,   747,   747,   747,   747,   747,   747,
     747,   747,   747,   747,   747,   747,   747,   748,   748,   748,
     748,   748,   748,   748,   749,   749,   749,   749,   750,   750,
     750,   750,   751,   751,   752,   752,   753,   753,   754,   755,
     755,   755,   755,   755,   755,   755,   755,   755,   755,   755,
     755,   755,   755,   755,   755,   755,   755,   755,   755,   755,
     755,   756,   757,   758,   758,   758,   758,   759,   760,   760,
     760,   761,   762,   762,   762,   762,   762,   763,   764,   764,
     764,   764,   764,   764,   764,   764,   764,   765,   765,   765,
     766,   766,   766,   766,   766,   766,   766,   766,   766,   766,
     766,   766,   767,   767,   767,   767,   768,   768,   768,   768,
     768,   769,   770,   770,   770,   770,   770,   770,   770,   771,
     772,   772,   772,   772,   773,   774,   774,   774,   774,   774,
     774,   774,   774,   774,   775,   776,   776,   776,   776,   776,
     776,   776,   776,   776,   776,   777,   778,   778,   778,   778,
     778,   779,   780,   780,   781,   782,   782,   782,   783,   784,
     784,   784,   784,   785,   786,   786,   786,   786,   787,   787,
     787,   787,   788,   788,   788,   788,   788,   788,   788,   788,
     788,   788,   788,   788,   788,   788,   789,   789,   789,   790,
     790,   791,   791,   792,   792,   793,   793,   794,   794,   795,
     795,   796,   796,   796,   797,   798,   799,   799,   799,   799,
     799,   799,   799,   799,   799,   799,   800,   800,   800,   800,
     800,   800,   801,   801,   801,   801,   801,   802,   802,   802,
     802,   802,   802,   802,   802,   802,   802,   802,   802,   803,
     803,   804,   805,   805,   806,   807,   808,   808,   809,   809,
     809,   810,   810,   810,   810,   810,   810,   810,   810,   810,
     810,   810,   810,   810,   810,   810,   810,   810,   810,   811,
     811,   811,   812,   812,   812,   812,   813,   813,   813,   813,
     813,   814,   814,   814,   814,   815,   815,   815,   815,   815,
     815,   815,   815,   815,   815,   815,   815,   816,   816,   817,
     817,   817,   817,   818,   819,   819,   820,   820,   820,   820,
     820,   820,   820,   820,   821,   822,   822,   823,   824,   824,
     824,   824,   825,   826,   827,   828,   829,   830,   830,   830,
     830,   831,   832,   832,   832,   832,   832,   832,   833,   834,
     834,   835,   836,   836,   836,   836,   836,   837,   838,   838,
     838,   838,   838
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     0,     2,     1,     1,     1,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     3,     5,     5,     3,     2,     1,     1,     2,
       2,     1,     2,     2,     2,     2,     2,     2,     3,     3,
       2,     2,     3,     3,     3,     2,     3,     2,     6,     2,
       6,     3,     2,     6,     6,     3,     6,     3,     5,     7,
       5,     7,     8,     8,     8,     5,     7,     5,     7,     5,
       7,     3,     2,     6,     2,     6,     6,     6,     3,     6,
       3,     5,     5,     8,     8,     8,     5,     5,     5,     3,
       1,     1,     1,     1,     1,     1,     1,     1,     2,     2,
       6,     2,     2,     2,     3,     2,     2,     6,     3,     3,
       5,     3,     3,     3,     2,     2,     2,     2,     2,     3,
       2,     2,     6,     3,     3,     5,     3,     3,     3,     3,
       2,     1,     1,     1,     2,     2,     2,     2,     2,     2,
       2,     3,     2,     3,     2,     3,     2,     3,     2,     3,
       2,     3,     3,     2,     2,     2,     2,     2,     2,     4,
       5,     2,     2,     1,     2,     2,     3,     2,     3,     2,
       2,     2,     3,     2,     3,     2,     2,     2,     2,     2,
       2,     3,     2,     2,     3,     2,     1,     2,     1,     3,
       0,     1,     0,     1,     1,     1,     1,     1,     1,     1,
       0,     1,     1,     1,     2,     1,     0,     2,     1,     1,
       3,     1,     0,     2,     2,     3,     8,     8,     8,     8,
       9,     9,    10,    10,    10,     9,     9,     9,     0,     0,
       2,     2,     3,     3,     3,     3,     5,     3,     0,     2,
       3,     1,     3,     2,     1,     1,     1,     1,     1,     3,
       0,     2,     3,     0,     2,     2,     3,     4,     4,     4,
       3,     4,     2,     3,     3,     1,     1,     3,     1,     1,
       1,     1,     1,     0,     1,     1,     2,     2,     2,     2,
       2,     1,     3,     1,     0,     0,     2,     2,     4,     4,
       8,     6,     7,     6,     4,     3,     4,     3,     3,     3,
       2,     1,     1,     0,     0,     2,     2,     5,     5,     3,
       4,     3,     1,     1,     3,     3,     1,     1,     1,     1,
       1,     1,     3,     0,     0,     2,     2,     2,     2,     0,
       2,     2,     2,     2,     0,     2,     2,     2,     2,     0,
       2,     2,     2,     2,     0,     2,     2,     2,     2,     0,
       2,     2,     2,     2,     0,     2,     2,     2,     2,     0,
       2,     2,     2,     2,     1,     1,     1,     1,     1,     7,
       2,     1,     1,     1,     1,     1,     3,     3,     1,     2,
       2,     2,     3,     0,     2,     3,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     0,     1,
       2,     2,     1,     2,     1,     1,     2,     3,     2,     3,
       1,     2,     3,     1,     2,     3,     1,     2,     3,     1,
       2,     2,     2,     1,     2,     2,     2,     2,     2,     0,
       1,     1,     2,     1,     1,     2,     1,     2,     4,     4,
       4,     4,     5,     5,     1,     1,     1,     1,     1,     2,
       2,     2,     2,     2,     1,     1,     1,     1,     1,     1,
       2,     2,     2,     2,     1,     1,     2,     2,     2,     2,
       1,     1,     2,     1,     1,     2,     1,     3,     1,     1,
       5,     1,     1,     3,     3,     1,     1,     3,     3,     5,
       2,     2,     1,     2,     1,     2,     1,     1,     2,     2,
       2,     1,     1,     2,     2,     2,     1,     2,     1,     1,
       2,     2,     2,     2,     2,     2,     2,     1,     3,     3,
       1,     2,     1,     3,     1,     1,     1,     2,     2,     3,
       3,     1,     1,     0,     1,     1,     0,     3,     1,     2,
       4,     1,     1,     0,     0,     3,     3,     0,     2,     2,
       3,     2,     2,     1,     1,     1,     1,     1,     2,     1,
       1,     1,     0,     6,     3,     6,     3,     5,     3,     5,
       2,     1,     1,     3,     4,     4,     5,     6,     5,     1,
       2,     1,     1,     2,     2,     2,     1,     1,     6,     8,
       0,     0,     1,     0,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     3,     1,     3,
       3,     1,     3,     1,     3,     1,     3,     1,     1,     3,
       1,     1,     3,     1,     3,     3,     1,     1,     1,     1,
       1,     1,     2,     3,     3,     4,     5,     2,     3,     2,
       6,     4,     3,     4,     3,     2,     1,     1,     3,     4,
       1,     2,     1,     1,     2,     3,     1,     3,     4,     3,
       5,     3,     6,     1,     3,     1,     1,     1,     2,     4,
       6,     6,     1,     2,     1,     1,     2,     2,     1,     1,
       1,     1,     1,     3,     1,     1,     1,     1,     1,     3,
       1,     1,     1,     2,     1,     4,     5,     6,     1,     1,
       1,     7,     8,     6,     8,     1,     2,     1,     1,     1,
       1,     3,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     3,     4,
       1,     1,     2,     1,     1,     1,     1,     1,     3,     1,
       4,     4,     0,     2,     1,     3,     3,     1,     3,     1,
       3,     1,     3,     1,     1,     3,     3,     3,     1,     1,
       3,     1,     1,     1,     3,     1,     3,     3,     3,     3,
       5,     1,     2,     1,     1,     2,     3,     1,     1,     2,
       1,     1,     2,     1,     2,     2,     1,     1,     2,     1,
       2,     2,     2,     2,     2,     2,     2,     2,     1,     1,
       1,     1,     1,     1,     2,     2,     4,     0,     4,     0,
       1,     0,     1,     1,     1,     1,     1,     1,     2,     2,
       6,     3,     1,     3,     3,     3,     7,     3,     3,     3,
       3,     3,     3,     0,     4,     4,     0,     2,     2,     4,
       4,     5,     5,     3,     3,     3,     3,     1,     1,     1,
       1,     3,     3,     1,     3,     1,     3,     1,     3,     1,
       1,     1,     3,     3,     1,     1,     1,     2,     2,     2,
       2,     2,     2,     2,     1,     2,     2,     1,     1,     1,
       2,     1,     1,     1,     1,     2,     2,     2,     2,     2,
       2,     1,     2,     2,     2,     2,     2,     2,     3,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     2,
       2,     1,     1,     1,     3,     1,     3,     1,     1,     1,
       1,     1,     1,     2,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     2,     1,     1,     1,     5,     3,
       5,     1,     5,     5,     3,     5,     1,     1,     1,     0,
       2,     1,     1,     6,     2,     0,     1,     1,     1,     1,
       1,     1,     5,     6,     8,     6,     5,     2,     2,     3,
       4,     1,     1,     1,     2,     3,     4,     4,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     2,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     3,     3,     3,     3,     1,     1,
       1,     1,     1,     1,     3,     3,     5,     6,     3,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     2,     1,     1,     1,     1,     1,     1,
       1,     1,     7,     1,     1,     2,     1,     3,     1,     1,
       2,     3,     1,     1,     1,     1,     2,     3,     1,     1,
       1,     1,     1,     3,     3,     3,     3,     3,     5,     4,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     2,     1,     1,     1,
       1,     3,     2,     1,     1,     1,     1,     1,     1,     3,
       2,     1,     1,     1,     3,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     3,     2,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     3,     1,     1,     1,     1,
       1,     3,     1,     1,     3,     1,     1,     1,     3,     1,
       1,     1,     1,     3,     1,     1,     1,     1,     2,     3,
       3,     9,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     2,     1,     1,     1,     1,     1,     1,     1,
       1,     2,     2,     1,     1,     2,     2,     1,     1,     3,
       3,     1,     1,     1,     3,     5,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     2,     2,     2,     2,
       2,     2,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     3,     4,     1,     1,
       2,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     2,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     3,     5,     1,
       1,     1,     1,     3,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     3,     1,     1,     3,     1,     1,
       2,     1,     3,     4,     3,     1,     3,     1,     1,     1,
       4,     3,     1,     1,     1,     1,     1,     1,     3,     1,
       1,     3,     1,     1,     2,     1,     1,     2,     2,     2,
       2,     2,     2
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYNOMEM         goto yyexhaustedlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (&yylloc, nft, scanner, state, YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF

/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)                                \
    do                                                                  \
      if (N)                                                            \
        {                                                               \
          (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;        \
          (Current).first_column = YYRHSLOC (Rhs, 1).first_column;      \
          (Current).last_line    = YYRHSLOC (Rhs, N).last_line;         \
          (Current).last_column  = YYRHSLOC (Rhs, N).last_column;       \
        }                                                               \
      else                                                              \
        {                                                               \
          (Current).first_line   = (Current).last_line   =              \
            YYRHSLOC (Rhs, 0).last_line;                                \
          (Current).first_column = (Current).last_column =              \
            YYRHSLOC (Rhs, 0).last_column;                              \
        }                                                               \
    while (0)
#endif

#define YYRHSLOC(Rhs, K) ((Rhs)[K])


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)


/* YYLOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

# ifndef YYLOCATION_PRINT

#  if defined YY_LOCATION_PRINT

   /* Temporary convenience wrapper in case some people defined the
      undocumented and private YY_LOCATION_PRINT macros.  */
#   define YYLOCATION_PRINT(File, Loc)  YY_LOCATION_PRINT(File, *(Loc))

#  elif defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL

/* Print *YYLOCP on YYO.  Private, do not rely on its existence. */

YY_ATTRIBUTE_UNUSED
static int
yy_location_print_ (FILE *yyo, YYLTYPE const * const yylocp)
{
  int res = 0;
  int end_col = 0 != yylocp->last_column ? yylocp->last_column - 1 : 0;
  if (0 <= yylocp->first_line)
    {
      res += YYFPRINTF (yyo, "%d", yylocp->first_line);
      if (0 <= yylocp->first_column)
        res += YYFPRINTF (yyo, ".%d", yylocp->first_column);
    }
  if (0 <= yylocp->last_line)
    {
      if (yylocp->first_line < yylocp->last_line)
        {
          res += YYFPRINTF (yyo, "-%d", yylocp->last_line);
          if (0 <= end_col)
            res += YYFPRINTF (yyo, ".%d", end_col);
        }
      else if (0 <= end_col && yylocp->first_column < end_col)
        res += YYFPRINTF (yyo, "-%d", end_col);
    }
  return res;
}

#   define YYLOCATION_PRINT  yy_location_print_

    /* Temporary convenience wrapper in case some people defined the
       undocumented and private YY_LOCATION_PRINT macros.  */
#   define YY_LOCATION_PRINT(File, Loc)  YYLOCATION_PRINT(File, &(Loc))

#  else

#   define YYLOCATION_PRINT(File, Loc) ((void) 0)
    /* Temporary convenience wrapper in case some people defined the
       undocumented and private YY_LOCATION_PRINT macros.  */
#   define YY_LOCATION_PRINT  YYLOCATION_PRINT

#  endif
# endif /* !defined YYLOCATION_PRINT */


# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value, Location, nft, scanner, state); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp, struct nft_ctx *nft, void *scanner, struct parser_state *state)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
  YY_USE (yylocationp);
  YY_USE (nft);
  YY_USE (scanner);
  YY_USE (state);
  if (!yyvaluep)
    return;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp, struct nft_ctx *nft, void *scanner, struct parser_state *state)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  YYLOCATION_PRINT (yyo, yylocationp);
  YYFPRINTF (yyo, ": ");
  yy_symbol_value_print (yyo, yykind, yyvaluep, yylocationp, nft, scanner, state);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp, YYLTYPE *yylsp,
                 int yyrule, struct nft_ctx *nft, void *scanner, struct parser_state *state)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)],
                       &(yylsp[(yyi + 1) - (yynrhs)]), nft, scanner, state);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, yylsp, Rule, nft, scanner, state); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif


/* Context of a parse error.  */
typedef struct
{
  yy_state_t *yyssp;
  yysymbol_kind_t yytoken;
  YYLTYPE *yylloc;
} yypcontext_t;

/* Put in YYARG at most YYARGN of the expected tokens given the
   current YYCTX, and return the number of tokens stored in YYARG.  If
   YYARG is null, return the number of expected tokens (guaranteed to
   be less than YYNTOKENS).  Return YYENOMEM on memory exhaustion.
   Return 0 if there are more than YYARGN expected tokens, yet fill
   YYARG up to YYARGN. */
static int
yypcontext_expected_tokens (const yypcontext_t *yyctx,
                            yysymbol_kind_t yyarg[], int yyargn)
{
  /* Actual size of YYARG. */
  int yycount = 0;
  int yyn = yypact[+*yyctx->yyssp];
  if (!yypact_value_is_default (yyn))
    {
      /* Start YYX at -YYN if negative to avoid negative indexes in
         YYCHECK.  In other words, skip the first -YYN actions for
         this state because they are default actions.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;
      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yyx;
      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
        if (yycheck[yyx + yyn] == yyx && yyx != YYSYMBOL_YYerror
            && !yytable_value_is_error (yytable[yyx + yyn]))
          {
            if (!yyarg)
              ++yycount;
            else if (yycount == yyargn)
              return 0;
            else
              yyarg[yycount++] = YY_CAST (yysymbol_kind_t, yyx);
          }
    }
  if (yyarg && yycount == 0 && 0 < yyargn)
    yyarg[0] = YYSYMBOL_YYEMPTY;
  return yycount;
}




#ifndef yystrlen
# if defined __GLIBC__ && defined _STRING_H
#  define yystrlen(S) (YY_CAST (YYPTRDIFF_T, strlen (S)))
# else
/* Return the length of YYSTR.  */
static YYPTRDIFF_T
yystrlen (const char *yystr)
{
  YYPTRDIFF_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
# endif
#endif

#ifndef yystpcpy
# if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#  define yystpcpy stpcpy
# else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
yystpcpy (char *yydest, const char *yysrc)
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
# endif
#endif

#ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYPTRDIFF_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYPTRDIFF_T yyn = 0;
      char const *yyp = yystr;
      for (;;)
        switch (*++yyp)
          {
          case '\'':
          case ',':
            goto do_not_strip_quotes;

          case '\\':
            if (*++yyp != '\\')
              goto do_not_strip_quotes;
            else
              goto append;

          append:
          default:
            if (yyres)
              yyres[yyn] = *yyp;
            yyn++;
            break;

          case '"':
            if (yyres)
              yyres[yyn] = '\0';
            return yyn;
          }
    do_not_strip_quotes: ;
    }

  if (yyres)
    return yystpcpy (yyres, yystr) - yyres;
  else
    return yystrlen (yystr);
}
#endif


static int
yy_syntax_error_arguments (const yypcontext_t *yyctx,
                           yysymbol_kind_t yyarg[], int yyargn)
{
  /* Actual size of YYARG. */
  int yycount = 0;
  /* There are many possibilities here to consider:
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yyctx->yytoken != YYSYMBOL_YYEMPTY)
    {
      int yyn;
      if (yyarg)
        yyarg[yycount] = yyctx->yytoken;
      ++yycount;
      yyn = yypcontext_expected_tokens (yyctx,
                                        yyarg ? yyarg + 1 : yyarg, yyargn - 1);
      if (yyn == YYENOMEM)
        return YYENOMEM;
      else
        yycount += yyn;
    }
  return yycount;
}

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return -1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return YYENOMEM if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYPTRDIFF_T *yymsg_alloc, char **yymsg,
                const yypcontext_t *yyctx)
{
  enum { YYARGS_MAX = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat: reported tokens (one for the "unexpected",
     one per "expected"). */
  yysymbol_kind_t yyarg[YYARGS_MAX];
  /* Cumulated lengths of YYARG.  */
  YYPTRDIFF_T yysize = 0;

  /* Actual size of YYARG. */
  int yycount = yy_syntax_error_arguments (yyctx, yyarg, YYARGS_MAX);
  if (yycount == YYENOMEM)
    return YYENOMEM;

  switch (yycount)
    {
#define YYCASE_(N, S)                       \
      case N:                               \
        yyformat = S;                       \
        break
    default: /* Avoid compiler warnings. */
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
#undef YYCASE_
    }

  /* Compute error message size.  Don't count the "%s"s, but reserve
     room for the terminator.  */
  yysize = yystrlen (yyformat) - 2 * yycount + 1;
  {
    int yyi;
    for (yyi = 0; yyi < yycount; ++yyi)
      {
        YYPTRDIFF_T yysize1
          = yysize + yytnamerr (YY_NULLPTR, yytname[yyarg[yyi]]);
        if (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM)
          yysize = yysize1;
        else
          return YYENOMEM;
      }
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return -1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yytname[yyarg[yyi++]]);
          yyformat += 2;
        }
      else
        {
          ++yyp;
          ++yyformat;
        }
  }
  return 0;
}


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep, YYLTYPE *yylocationp, struct nft_ctx *nft, void *scanner, struct parser_state *state)
{
  YY_USE (yyvaluep);
  YY_USE (yylocationp);
  YY_USE (nft);
  YY_USE (scanner);
  YY_USE (state);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  switch (yykind)
    {
    case YYSYMBOL_STRING: /* "string"  */
#line 379 "src/parser_bison.y"
            { free_const(((*yyvaluep).string)); }
#line 6263 "src/parser_bison.c"
        break;

    case YYSYMBOL_QUOTED_STRING: /* "quoted string"  */
#line 379 "src/parser_bison.y"
            { free_const(((*yyvaluep).string)); }
#line 6269 "src/parser_bison.c"
        break;

    case YYSYMBOL_ASTERISK_STRING: /* "string with a trailing asterisk"  */
#line 379 "src/parser_bison.y"
            { free_const(((*yyvaluep).string)); }
#line 6275 "src/parser_bison.c"
        break;

    case YYSYMBOL_line: /* line  */
#line 703 "src/parser_bison.y"
            { cmd_free(((*yyvaluep).cmd)); }
#line 6281 "src/parser_bison.c"
        break;

    case YYSYMBOL_base_cmd: /* base_cmd  */
#line 706 "src/parser_bison.y"
            { cmd_free(((*yyvaluep).cmd)); }
#line 6287 "src/parser_bison.c"
        break;

    case YYSYMBOL_add_cmd: /* add_cmd  */
#line 706 "src/parser_bison.y"
            { cmd_free(((*yyvaluep).cmd)); }
#line 6293 "src/parser_bison.c"
        break;

    case YYSYMBOL_replace_cmd: /* replace_cmd  */
#line 706 "src/parser_bison.y"
            { cmd_free(((*yyvaluep).cmd)); }
#line 6299 "src/parser_bison.c"
        break;

    case YYSYMBOL_create_cmd: /* create_cmd  */
#line 706 "src/parser_bison.y"
            { cmd_free(((*yyvaluep).cmd)); }
#line 6305 "src/parser_bison.c"
        break;

    case YYSYMBOL_insert_cmd: /* insert_cmd  */
#line 706 "src/parser_bison.y"
            { cmd_free(((*yyvaluep).cmd)); }
#line 6311 "src/parser_bison.c"
        break;

    case YYSYMBOL_table_or_id_spec: /* table_or_id_spec  */
#line 709 "src/parser_bison.y"
            { handle_free(&((*yyvaluep).handle)); }
#line 6317 "src/parser_bison.c"
        break;

    case YYSYMBOL_chain_or_id_spec: /* chain_or_id_spec  */
#line 711 "src/parser_bison.y"
            { handle_free(&((*yyvaluep).handle)); }
#line 6323 "src/parser_bison.c"
        break;

    case YYSYMBOL_set_or_id_spec: /* set_or_id_spec  */
#line 716 "src/parser_bison.y"
            { handle_free(&((*yyvaluep).handle)); }
#line 6329 "src/parser_bison.c"
        break;

    case YYSYMBOL_obj_or_id_spec: /* obj_or_id_spec  */
#line 718 "src/parser_bison.y"
            { handle_free(&((*yyvaluep).handle)); }
#line 6335 "src/parser_bison.c"
        break;

    case YYSYMBOL_delete_cmd: /* delete_cmd  */
#line 706 "src/parser_bison.y"
            { cmd_free(((*yyvaluep).cmd)); }
#line 6341 "src/parser_bison.c"
        break;

    case YYSYMBOL_destroy_cmd: /* destroy_cmd  */
#line 706 "src/parser_bison.y"
            { cmd_free(((*yyvaluep).cmd)); }
#line 6347 "src/parser_bison.c"
        break;

    case YYSYMBOL_get_cmd: /* get_cmd  */
#line 706 "src/parser_bison.y"
            { cmd_free(((*yyvaluep).cmd)); }
#line 6353 "src/parser_bison.c"
        break;

    case YYSYMBOL_list_cmd_spec_table: /* list_cmd_spec_table  */
#line 727 "src/parser_bison.y"
            { handle_free(&((*yyvaluep).handle)); }
#line 6359 "src/parser_bison.c"
        break;

    case YYSYMBOL_list_cmd_spec_any: /* list_cmd_spec_any  */
#line 727 "src/parser_bison.y"
            { handle_free(&((*yyvaluep).handle)); }
#line 6365 "src/parser_bison.c"
        break;

    case YYSYMBOL_list_cmd: /* list_cmd  */
#line 706 "src/parser_bison.y"
            { cmd_free(((*yyvaluep).cmd)); }
#line 6371 "src/parser_bison.c"
        break;

    case YYSYMBOL_basehook_device_name: /* basehook_device_name  */
#line 735 "src/parser_bison.y"
            { free_const(((*yyvaluep).string)); }
#line 6377 "src/parser_bison.c"
        break;

    case YYSYMBOL_basehook_spec: /* basehook_spec  */
#line 724 "src/parser_bison.y"
            { handle_free(&((*yyvaluep).handle)); }
#line 6383 "src/parser_bison.c"
        break;

    case YYSYMBOL_reset_cmd: /* reset_cmd  */
#line 706 "src/parser_bison.y"
            { cmd_free(((*yyvaluep).cmd)); }
#line 6389 "src/parser_bison.c"
        break;

    case YYSYMBOL_flush_cmd: /* flush_cmd  */
#line 706 "src/parser_bison.y"
            { cmd_free(((*yyvaluep).cmd)); }
#line 6395 "src/parser_bison.c"
        break;

    case YYSYMBOL_rename_cmd: /* rename_cmd  */
#line 706 "src/parser_bison.y"
            { cmd_free(((*yyvaluep).cmd)); }
#line 6401 "src/parser_bison.c"
        break;

    case YYSYMBOL_import_cmd: /* import_cmd  */
#line 706 "src/parser_bison.y"
            { cmd_free(((*yyvaluep).cmd)); }
#line 6407 "src/parser_bison.c"
        break;

    case YYSYMBOL_export_cmd: /* export_cmd  */
#line 706 "src/parser_bison.y"
            { cmd_free(((*yyvaluep).cmd)); }
#line 6413 "src/parser_bison.c"
        break;

    case YYSYMBOL_monitor_cmd: /* monitor_cmd  */
#line 706 "src/parser_bison.y"
            { cmd_free(((*yyvaluep).cmd)); }
#line 6419 "src/parser_bison.c"
        break;

    case YYSYMBOL_monitor_event: /* monitor_event  */
#line 963 "src/parser_bison.y"
            { free_const(((*yyvaluep).string)); }
#line 6425 "src/parser_bison.c"
        break;

    case YYSYMBOL_describe_cmd: /* describe_cmd  */
#line 706 "src/parser_bison.y"
            { cmd_free(((*yyvaluep).cmd)); }
#line 6431 "src/parser_bison.c"
        break;

    case YYSYMBOL_table_block_alloc: /* table_block_alloc  */
#line 741 "src/parser_bison.y"
            { close_scope(state); table_free(((*yyvaluep).table)); }
#line 6437 "src/parser_bison.c"
        break;

    case YYSYMBOL_chain_block_alloc: /* chain_block_alloc  */
#line 743 "src/parser_bison.y"
            { close_scope(state); chain_free(((*yyvaluep).chain)); }
#line 6443 "src/parser_bison.c"
        break;

    case YYSYMBOL_typeof_verdict_expr: /* typeof_verdict_expr  */
#line 822 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 6449 "src/parser_bison.c"
        break;

    case YYSYMBOL_typeof_data_expr: /* typeof_data_expr  */
#line 822 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 6455 "src/parser_bison.c"
        break;

    case YYSYMBOL_primary_typeof_expr: /* primary_typeof_expr  */
#line 822 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 6461 "src/parser_bison.c"
        break;

    case YYSYMBOL_typeof_expr: /* typeof_expr  */
#line 822 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 6467 "src/parser_bison.c"
        break;

    case YYSYMBOL_set_block_alloc: /* set_block_alloc  */
#line 754 "src/parser_bison.y"
            { set_free(((*yyvaluep).set)); }
#line 6473 "src/parser_bison.c"
        break;

    case YYSYMBOL_typeof_key_expr: /* typeof_key_expr  */
#line 822 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 6479 "src/parser_bison.c"
        break;

    case YYSYMBOL_set_block_expr: /* set_block_expr  */
#line 863 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 6485 "src/parser_bison.c"
        break;

    case YYSYMBOL_map_block_alloc: /* map_block_alloc  */
#line 757 "src/parser_bison.y"
            { set_free(((*yyvaluep).set)); }
#line 6491 "src/parser_bison.c"
        break;

    case YYSYMBOL_flowtable_block_alloc: /* flowtable_block_alloc  */
#line 761 "src/parser_bison.y"
            { flowtable_free(((*yyvaluep).flowtable)); }
#line 6497 "src/parser_bison.c"
        break;

    case YYSYMBOL_flowtable_expr: /* flowtable_expr  */
#line 863 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 6503 "src/parser_bison.c"
        break;

    case YYSYMBOL_flowtable_list_expr: /* flowtable_list_expr  */
#line 863 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 6509 "src/parser_bison.c"
        break;

    case YYSYMBOL_flowtable_expr_member: /* flowtable_expr_member  */
#line 863 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 6515 "src/parser_bison.c"
        break;

    case YYSYMBOL_data_type_atom_expr: /* data_type_atom_expr  */
#line 700 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 6521 "src/parser_bison.c"
        break;

    case YYSYMBOL_data_type_expr: /* data_type_expr  */
#line 700 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 6527 "src/parser_bison.c"
        break;

    case YYSYMBOL_obj_block_alloc: /* obj_block_alloc  */
#line 764 "src/parser_bison.y"
            { obj_free(((*yyvaluep).obj)); }
#line 6533 "src/parser_bison.c"
        break;

    case YYSYMBOL_type_identifier: /* type_identifier  */
#line 695 "src/parser_bison.y"
            { free_const(((*yyvaluep).string)); }
#line 6539 "src/parser_bison.c"
        break;

    case YYSYMBOL_prio_spec: /* prio_spec  */
#line 732 "src/parser_bison.y"
            { expr_free(((*yyvaluep).prio_spec).expr); }
#line 6545 "src/parser_bison.c"
        break;

    case YYSYMBOL_extended_prio_name: /* extended_prio_name  */
#line 735 "src/parser_bison.y"
            { free_const(((*yyvaluep).string)); }
#line 6551 "src/parser_bison.c"
        break;

    case YYSYMBOL_extended_prio_spec: /* extended_prio_spec  */
#line 732 "src/parser_bison.y"
            { expr_free(((*yyvaluep).prio_spec).expr); }
#line 6557 "src/parser_bison.c"
        break;

    case YYSYMBOL_dev_spec: /* dev_spec  */
#line 738 "src/parser_bison.y"
            { free(((*yyvaluep).expr)); }
#line 6563 "src/parser_bison.c"
        break;

    case YYSYMBOL_policy_expr: /* policy_expr  */
#line 820 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 6569 "src/parser_bison.c"
        break;

    case YYSYMBOL_identifier: /* identifier  */
#line 695 "src/parser_bison.y"
            { free_const(((*yyvaluep).string)); }
#line 6575 "src/parser_bison.c"
        break;

    case YYSYMBOL_string: /* string  */
#line 695 "src/parser_bison.y"
            { free_const(((*yyvaluep).string)); }
#line 6581 "src/parser_bison.c"
        break;

    case YYSYMBOL_table_spec: /* table_spec  */
#line 709 "src/parser_bison.y"
            { handle_free(&((*yyvaluep).handle)); }
#line 6587 "src/parser_bison.c"
        break;

    case YYSYMBOL_tableid_spec: /* tableid_spec  */
#line 709 "src/parser_bison.y"
            { handle_free(&((*yyvaluep).handle)); }
#line 6593 "src/parser_bison.c"
        break;

    case YYSYMBOL_chain_spec: /* chain_spec  */
#line 711 "src/parser_bison.y"
            { handle_free(&((*yyvaluep).handle)); }
#line 6599 "src/parser_bison.c"
        break;

    case YYSYMBOL_chainid_spec: /* chainid_spec  */
#line 711 "src/parser_bison.y"
            { handle_free(&((*yyvaluep).handle)); }
#line 6605 "src/parser_bison.c"
        break;

    case YYSYMBOL_chain_identifier: /* chain_identifier  */
#line 714 "src/parser_bison.y"
            { handle_free(&((*yyvaluep).handle)); }
#line 6611 "src/parser_bison.c"
        break;

    case YYSYMBOL_set_spec: /* set_spec  */
#line 716 "src/parser_bison.y"
            { handle_free(&((*yyvaluep).handle)); }
#line 6617 "src/parser_bison.c"
        break;

    case YYSYMBOL_setid_spec: /* setid_spec  */
#line 716 "src/parser_bison.y"
            { handle_free(&((*yyvaluep).handle)); }
#line 6623 "src/parser_bison.c"
        break;

    case YYSYMBOL_set_identifier: /* set_identifier  */
#line 721 "src/parser_bison.y"
            { handle_free(&((*yyvaluep).handle)); }
#line 6629 "src/parser_bison.c"
        break;

    case YYSYMBOL_flowtable_spec: /* flowtable_spec  */
#line 714 "src/parser_bison.y"
            { handle_free(&((*yyvaluep).handle)); }
#line 6635 "src/parser_bison.c"
        break;

    case YYSYMBOL_flowtableid_spec: /* flowtableid_spec  */
#line 721 "src/parser_bison.y"
            { handle_free(&((*yyvaluep).handle)); }
#line 6641 "src/parser_bison.c"
        break;

    case YYSYMBOL_obj_spec: /* obj_spec  */
#line 718 "src/parser_bison.y"
            { handle_free(&((*yyvaluep).handle)); }
#line 6647 "src/parser_bison.c"
        break;

    case YYSYMBOL_objid_spec: /* objid_spec  */
#line 718 "src/parser_bison.y"
            { handle_free(&((*yyvaluep).handle)); }
#line 6653 "src/parser_bison.c"
        break;

    case YYSYMBOL_obj_identifier: /* obj_identifier  */
#line 721 "src/parser_bison.y"
            { handle_free(&((*yyvaluep).handle)); }
#line 6659 "src/parser_bison.c"
        break;

    case YYSYMBOL_handle_spec: /* handle_spec  */
#line 714 "src/parser_bison.y"
            { handle_free(&((*yyvaluep).handle)); }
#line 6665 "src/parser_bison.c"
        break;

    case YYSYMBOL_position_spec: /* position_spec  */
#line 714 "src/parser_bison.y"
            { handle_free(&((*yyvaluep).handle)); }
#line 6671 "src/parser_bison.c"
        break;

    case YYSYMBOL_index_spec: /* index_spec  */
#line 714 "src/parser_bison.y"
            { handle_free(&((*yyvaluep).handle)); }
#line 6677 "src/parser_bison.c"
        break;

    case YYSYMBOL_rule_position: /* rule_position  */
#line 714 "src/parser_bison.y"
            { handle_free(&((*yyvaluep).handle)); }
#line 6683 "src/parser_bison.c"
        break;

    case YYSYMBOL_ruleid_spec: /* ruleid_spec  */
#line 714 "src/parser_bison.y"
            { handle_free(&((*yyvaluep).handle)); }
#line 6689 "src/parser_bison.c"
        break;

    case YYSYMBOL_comment_spec: /* comment_spec  */
#line 695 "src/parser_bison.y"
            { free_const(((*yyvaluep).string)); }
#line 6695 "src/parser_bison.c"
        break;

    case YYSYMBOL_ruleset_spec: /* ruleset_spec  */
#line 714 "src/parser_bison.y"
            { handle_free(&((*yyvaluep).handle)); }
#line 6701 "src/parser_bison.c"
        break;

    case YYSYMBOL_rule: /* rule  */
#line 745 "src/parser_bison.y"
            { rule_free(((*yyvaluep).rule)); }
#line 6707 "src/parser_bison.c"
        break;

    case YYSYMBOL_stmt_list: /* stmt_list  */
#line 767 "src/parser_bison.y"
            { stmt_list_free(((*yyvaluep).list)); free(((*yyvaluep).list)); }
#line 6713 "src/parser_bison.c"
        break;

    case YYSYMBOL_stateful_stmt_list: /* stateful_stmt_list  */
#line 767 "src/parser_bison.y"
            { stmt_list_free(((*yyvaluep).list)); free(((*yyvaluep).list)); }
#line 6719 "src/parser_bison.c"
        break;

    case YYSYMBOL_objref_stmt_counter: /* objref_stmt_counter  */
#line 775 "src/parser_bison.y"
            { stmt_free(((*yyvaluep).stmt)); }
#line 6725 "src/parser_bison.c"
        break;

    case YYSYMBOL_objref_stmt_limit: /* objref_stmt_limit  */
#line 775 "src/parser_bison.y"
            { stmt_free(((*yyvaluep).stmt)); }
#line 6731 "src/parser_bison.c"
        break;

    case YYSYMBOL_objref_stmt_quota: /* objref_stmt_quota  */
#line 775 "src/parser_bison.y"
            { stmt_free(((*yyvaluep).stmt)); }
#line 6737 "src/parser_bison.c"
        break;

    case YYSYMBOL_objref_stmt_synproxy: /* objref_stmt_synproxy  */
#line 775 "src/parser_bison.y"
            { stmt_free(((*yyvaluep).stmt)); }
#line 6743 "src/parser_bison.c"
        break;

    case YYSYMBOL_objref_stmt_ct: /* objref_stmt_ct  */
#line 775 "src/parser_bison.y"
            { stmt_free(((*yyvaluep).stmt)); }
#line 6749 "src/parser_bison.c"
        break;

    case YYSYMBOL_objref_stmt: /* objref_stmt  */
#line 775 "src/parser_bison.y"
            { stmt_free(((*yyvaluep).stmt)); }
#line 6755 "src/parser_bison.c"
        break;

    case YYSYMBOL_stateful_stmt: /* stateful_stmt  */
#line 771 "src/parser_bison.y"
            { stmt_free(((*yyvaluep).stmt)); }
#line 6761 "src/parser_bison.c"
        break;

    case YYSYMBOL_stmt: /* stmt  */
#line 769 "src/parser_bison.y"
            { stmt_free(((*yyvaluep).stmt)); }
#line 6767 "src/parser_bison.c"
        break;

    case YYSYMBOL_xt_stmt: /* xt_stmt  */
#line 987 "src/parser_bison.y"
            { stmt_free(((*yyvaluep).stmt)); }
#line 6773 "src/parser_bison.c"
        break;

    case YYSYMBOL_chain_stmt: /* chain_stmt  */
#line 799 "src/parser_bison.y"
            { stmt_free(((*yyvaluep).stmt)); }
#line 6779 "src/parser_bison.c"
        break;

    case YYSYMBOL_verdict_stmt: /* verdict_stmt  */
#line 769 "src/parser_bison.y"
            { stmt_free(((*yyvaluep).stmt)); }
#line 6785 "src/parser_bison.c"
        break;

    case YYSYMBOL_verdict_map_stmt: /* verdict_map_stmt  */
#line 857 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 6791 "src/parser_bison.c"
        break;

    case YYSYMBOL_verdict_map_expr: /* verdict_map_expr  */
#line 860 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 6797 "src/parser_bison.c"
        break;

    case YYSYMBOL_verdict_map_list_expr: /* verdict_map_list_expr  */
#line 860 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 6803 "src/parser_bison.c"
        break;

    case YYSYMBOL_verdict_map_list_member_expr: /* verdict_map_list_member_expr  */
#line 860 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 6809 "src/parser_bison.c"
        break;

    case YYSYMBOL_ct_limit_stmt_alloc: /* ct_limit_stmt_alloc  */
#line 773 "src/parser_bison.y"
            { stmt_free(((*yyvaluep).stmt)); }
#line 6815 "src/parser_bison.c"
        break;

    case YYSYMBOL_connlimit_stmt: /* connlimit_stmt  */
#line 787 "src/parser_bison.y"
            { stmt_free(((*yyvaluep).stmt)); }
#line 6821 "src/parser_bison.c"
        break;

    case YYSYMBOL_counter_stmt: /* counter_stmt  */
#line 771 "src/parser_bison.y"
            { stmt_free(((*yyvaluep).stmt)); }
#line 6827 "src/parser_bison.c"
        break;

    case YYSYMBOL_counter_stmt_alloc: /* counter_stmt_alloc  */
#line 771 "src/parser_bison.y"
            { stmt_free(((*yyvaluep).stmt)); }
#line 6833 "src/parser_bison.c"
        break;

    case YYSYMBOL_last_stmt_alloc: /* last_stmt_alloc  */
#line 773 "src/parser_bison.y"
            { stmt_free(((*yyvaluep).stmt)); }
#line 6839 "src/parser_bison.c"
        break;

    case YYSYMBOL_last_stmt: /* last_stmt  */
#line 771 "src/parser_bison.y"
            { stmt_free(((*yyvaluep).stmt)); }
#line 6845 "src/parser_bison.c"
        break;

    case YYSYMBOL_log_stmt: /* log_stmt  */
#line 784 "src/parser_bison.y"
            { stmt_free(((*yyvaluep).stmt)); }
#line 6851 "src/parser_bison.c"
        break;

    case YYSYMBOL_log_stmt_alloc: /* log_stmt_alloc  */
#line 784 "src/parser_bison.y"
            { stmt_free(((*yyvaluep).stmt)); }
#line 6857 "src/parser_bison.c"
        break;

    case YYSYMBOL_limit_stmt_alloc: /* limit_stmt_alloc  */
#line 773 "src/parser_bison.y"
            { stmt_free(((*yyvaluep).stmt)); }
#line 6863 "src/parser_bison.c"
        break;

    case YYSYMBOL_limit_stmt: /* limit_stmt  */
#line 787 "src/parser_bison.y"
            { stmt_free(((*yyvaluep).stmt)); }
#line 6869 "src/parser_bison.c"
        break;

    case YYSYMBOL_quota_unit: /* quota_unit  */
#line 735 "src/parser_bison.y"
            { free_const(((*yyvaluep).string)); }
#line 6875 "src/parser_bison.c"
        break;

    case YYSYMBOL_quota_stmt_alloc: /* quota_stmt_alloc  */
#line 773 "src/parser_bison.y"
            { stmt_free(((*yyvaluep).stmt)); }
#line 6881 "src/parser_bison.c"
        break;

    case YYSYMBOL_quota_stmt: /* quota_stmt  */
#line 787 "src/parser_bison.y"
            { stmt_free(((*yyvaluep).stmt)); }
#line 6887 "src/parser_bison.c"
        break;

    case YYSYMBOL_reject_stmt: /* reject_stmt  */
#line 790 "src/parser_bison.y"
            { stmt_free(((*yyvaluep).stmt)); }
#line 6893 "src/parser_bison.c"
        break;

    case YYSYMBOL_reject_stmt_alloc: /* reject_stmt_alloc  */
#line 790 "src/parser_bison.y"
            { stmt_free(((*yyvaluep).stmt)); }
#line 6899 "src/parser_bison.c"
        break;

    case YYSYMBOL_reject_with_expr: /* reject_with_expr  */
#line 805 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 6905 "src/parser_bison.c"
        break;

    case YYSYMBOL_nat_stmt: /* nat_stmt  */
#line 792 "src/parser_bison.y"
            { stmt_free(((*yyvaluep).stmt)); }
#line 6911 "src/parser_bison.c"
        break;

    case YYSYMBOL_nat_stmt_alloc: /* nat_stmt_alloc  */
#line 792 "src/parser_bison.y"
            { stmt_free(((*yyvaluep).stmt)); }
#line 6917 "src/parser_bison.c"
        break;

    case YYSYMBOL_tproxy_stmt: /* tproxy_stmt  */
#line 795 "src/parser_bison.y"
            { stmt_free(((*yyvaluep).stmt)); }
#line 6923 "src/parser_bison.c"
        break;

    case YYSYMBOL_synproxy_stmt: /* synproxy_stmt  */
#line 797 "src/parser_bison.y"
            { stmt_free(((*yyvaluep).stmt)); }
#line 6929 "src/parser_bison.c"
        break;

    case YYSYMBOL_synproxy_stmt_alloc: /* synproxy_stmt_alloc  */
#line 797 "src/parser_bison.y"
            { stmt_free(((*yyvaluep).stmt)); }
#line 6935 "src/parser_bison.c"
        break;

    case YYSYMBOL_synproxy_obj: /* synproxy_obj  */
#line 883 "src/parser_bison.y"
            { obj_free(((*yyvaluep).obj)); }
#line 6941 "src/parser_bison.c"
        break;

    case YYSYMBOL_primary_stmt_expr: /* primary_stmt_expr  */
#line 844 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 6947 "src/parser_bison.c"
        break;

    case YYSYMBOL_shift_stmt_expr: /* shift_stmt_expr  */
#line 846 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 6953 "src/parser_bison.c"
        break;

    case YYSYMBOL_and_stmt_expr: /* and_stmt_expr  */
#line 848 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 6959 "src/parser_bison.c"
        break;

    case YYSYMBOL_exclusive_or_stmt_expr: /* exclusive_or_stmt_expr  */
#line 848 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 6965 "src/parser_bison.c"
        break;

    case YYSYMBOL_inclusive_or_stmt_expr: /* inclusive_or_stmt_expr  */
#line 848 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 6971 "src/parser_bison.c"
        break;

    case YYSYMBOL_basic_stmt_expr: /* basic_stmt_expr  */
#line 844 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 6977 "src/parser_bison.c"
        break;

    case YYSYMBOL_concat_stmt_expr: /* concat_stmt_expr  */
#line 836 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 6983 "src/parser_bison.c"
        break;

    case YYSYMBOL_map_stmt_expr_set: /* map_stmt_expr_set  */
#line 836 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 6989 "src/parser_bison.c"
        break;

    case YYSYMBOL_map_stmt_expr: /* map_stmt_expr  */
#line 836 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 6995 "src/parser_bison.c"
        break;

    case YYSYMBOL_prefix_stmt_expr: /* prefix_stmt_expr  */
#line 841 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7001 "src/parser_bison.c"
        break;

    case YYSYMBOL_range_stmt_expr: /* range_stmt_expr  */
#line 841 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7007 "src/parser_bison.c"
        break;

    case YYSYMBOL_multiton_stmt_expr: /* multiton_stmt_expr  */
#line 839 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7013 "src/parser_bison.c"
        break;

    case YYSYMBOL_stmt_expr: /* stmt_expr  */
#line 836 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7019 "src/parser_bison.c"
        break;

    case YYSYMBOL_masq_stmt: /* masq_stmt  */
#line 792 "src/parser_bison.y"
            { stmt_free(((*yyvaluep).stmt)); }
#line 7025 "src/parser_bison.c"
        break;

    case YYSYMBOL_masq_stmt_alloc: /* masq_stmt_alloc  */
#line 792 "src/parser_bison.y"
            { stmt_free(((*yyvaluep).stmt)); }
#line 7031 "src/parser_bison.c"
        break;

    case YYSYMBOL_redir_stmt: /* redir_stmt  */
#line 792 "src/parser_bison.y"
            { stmt_free(((*yyvaluep).stmt)); }
#line 7037 "src/parser_bison.c"
        break;

    case YYSYMBOL_redir_stmt_alloc: /* redir_stmt_alloc  */
#line 792 "src/parser_bison.y"
            { stmt_free(((*yyvaluep).stmt)); }
#line 7043 "src/parser_bison.c"
        break;

    case YYSYMBOL_dup_stmt: /* dup_stmt  */
#line 808 "src/parser_bison.y"
            { stmt_free(((*yyvaluep).stmt)); }
#line 7049 "src/parser_bison.c"
        break;

    case YYSYMBOL_fwd_stmt: /* fwd_stmt  */
#line 810 "src/parser_bison.y"
            { stmt_free(((*yyvaluep).stmt)); }
#line 7055 "src/parser_bison.c"
        break;

    case YYSYMBOL_queue_stmt: /* queue_stmt  */
#line 803 "src/parser_bison.y"
            { stmt_free(((*yyvaluep).stmt)); }
#line 7061 "src/parser_bison.c"
        break;

    case YYSYMBOL_queue_stmt_compat: /* queue_stmt_compat  */
#line 803 "src/parser_bison.y"
            { stmt_free(((*yyvaluep).stmt)); }
#line 7067 "src/parser_bison.c"
        break;

    case YYSYMBOL_queue_stmt_alloc: /* queue_stmt_alloc  */
#line 803 "src/parser_bison.y"
            { stmt_free(((*yyvaluep).stmt)); }
#line 7073 "src/parser_bison.c"
        break;

    case YYSYMBOL_queue_expr: /* queue_expr  */
#line 805 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7079 "src/parser_bison.c"
        break;

    case YYSYMBOL_queue_stmt_expr_simple: /* queue_stmt_expr_simple  */
#line 805 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7085 "src/parser_bison.c"
        break;

    case YYSYMBOL_queue_stmt_expr: /* queue_stmt_expr  */
#line 805 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7091 "src/parser_bison.c"
        break;

    case YYSYMBOL_set_elem_expr_stmt: /* set_elem_expr_stmt  */
#line 867 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7097 "src/parser_bison.c"
        break;

    case YYSYMBOL_set_elem_expr_stmt_alloc: /* set_elem_expr_stmt_alloc  */
#line 867 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7103 "src/parser_bison.c"
        break;

    case YYSYMBOL_set_stmt: /* set_stmt  */
#line 812 "src/parser_bison.y"
            { stmt_free(((*yyvaluep).stmt)); }
#line 7109 "src/parser_bison.c"
        break;

    case YYSYMBOL_map_stmt: /* map_stmt  */
#line 815 "src/parser_bison.y"
            { stmt_free(((*yyvaluep).stmt)); }
#line 7115 "src/parser_bison.c"
        break;

    case YYSYMBOL_meter_stmt: /* meter_stmt  */
#line 817 "src/parser_bison.y"
            { stmt_free(((*yyvaluep).stmt)); }
#line 7121 "src/parser_bison.c"
        break;

    case YYSYMBOL_match_stmt: /* match_stmt  */
#line 769 "src/parser_bison.y"
            { stmt_free(((*yyvaluep).stmt)); }
#line 7127 "src/parser_bison.c"
        break;

    case YYSYMBOL_variable_expr: /* variable_expr  */
#line 820 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7133 "src/parser_bison.c"
        break;

    case YYSYMBOL_symbol_expr: /* symbol_expr  */
#line 820 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7139 "src/parser_bison.c"
        break;

    case YYSYMBOL_set_ref_expr: /* set_ref_expr  */
#line 828 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7145 "src/parser_bison.c"
        break;

    case YYSYMBOL_set_ref_symbol_expr: /* set_ref_symbol_expr  */
#line 828 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7151 "src/parser_bison.c"
        break;

    case YYSYMBOL_integer_expr: /* integer_expr  */
#line 820 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7157 "src/parser_bison.c"
        break;

    case YYSYMBOL_selector_expr: /* selector_expr  */
#line 822 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7163 "src/parser_bison.c"
        break;

    case YYSYMBOL_primary_expr: /* primary_expr  */
#line 822 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7169 "src/parser_bison.c"
        break;

    case YYSYMBOL_fib_expr: /* fib_expr  */
#line 954 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7175 "src/parser_bison.c"
        break;

    case YYSYMBOL_osf_expr: /* osf_expr  */
#line 959 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7181 "src/parser_bison.c"
        break;

    case YYSYMBOL_shift_expr: /* shift_expr  */
#line 822 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7187 "src/parser_bison.c"
        break;

    case YYSYMBOL_and_expr: /* and_expr  */
#line 822 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7193 "src/parser_bison.c"
        break;

    case YYSYMBOL_exclusive_or_expr: /* exclusive_or_expr  */
#line 824 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7199 "src/parser_bison.c"
        break;

    case YYSYMBOL_inclusive_or_expr: /* inclusive_or_expr  */
#line 824 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7205 "src/parser_bison.c"
        break;

    case YYSYMBOL_basic_expr: /* basic_expr  */
#line 826 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7211 "src/parser_bison.c"
        break;

    case YYSYMBOL_concat_expr: /* concat_expr  */
#line 851 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7217 "src/parser_bison.c"
        break;

    case YYSYMBOL_prefix_rhs_expr: /* prefix_rhs_expr  */
#line 833 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7223 "src/parser_bison.c"
        break;

    case YYSYMBOL_range_rhs_expr: /* range_rhs_expr  */
#line 833 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7229 "src/parser_bison.c"
        break;

    case YYSYMBOL_multiton_rhs_expr: /* multiton_rhs_expr  */
#line 831 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7235 "src/parser_bison.c"
        break;

    case YYSYMBOL_map_expr: /* map_expr  */
#line 854 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7241 "src/parser_bison.c"
        break;

    case YYSYMBOL_expr: /* expr  */
#line 873 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7247 "src/parser_bison.c"
        break;

    case YYSYMBOL_set_expr: /* set_expr  */
#line 863 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7253 "src/parser_bison.c"
        break;

    case YYSYMBOL_set_list_expr: /* set_list_expr  */
#line 863 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7259 "src/parser_bison.c"
        break;

    case YYSYMBOL_set_list_member_expr: /* set_list_member_expr  */
#line 863 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7265 "src/parser_bison.c"
        break;

    case YYSYMBOL_meter_key_expr: /* meter_key_expr  */
#line 870 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7271 "src/parser_bison.c"
        break;

    case YYSYMBOL_meter_key_expr_alloc: /* meter_key_expr_alloc  */
#line 870 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7277 "src/parser_bison.c"
        break;

    case YYSYMBOL_set_elem_expr: /* set_elem_expr  */
#line 865 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7283 "src/parser_bison.c"
        break;

    case YYSYMBOL_set_elem_key_expr: /* set_elem_key_expr  */
#line 1010 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7289 "src/parser_bison.c"
        break;

    case YYSYMBOL_set_elem_expr_alloc: /* set_elem_expr_alloc  */
#line 865 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7295 "src/parser_bison.c"
        break;

    case YYSYMBOL_set_elem_stmt_list: /* set_elem_stmt_list  */
#line 767 "src/parser_bison.y"
            { stmt_list_free(((*yyvaluep).list)); free(((*yyvaluep).list)); }
#line 7301 "src/parser_bison.c"
        break;

    case YYSYMBOL_set_elem_stmt: /* set_elem_stmt  */
#line 769 "src/parser_bison.y"
            { stmt_free(((*yyvaluep).stmt)); }
#line 7307 "src/parser_bison.c"
        break;

    case YYSYMBOL_set_lhs_expr: /* set_lhs_expr  */
#line 865 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7313 "src/parser_bison.c"
        break;

    case YYSYMBOL_set_rhs_expr: /* set_rhs_expr  */
#line 865 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7319 "src/parser_bison.c"
        break;

    case YYSYMBOL_initializer_expr: /* initializer_expr  */
#line 873 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7325 "src/parser_bison.c"
        break;

    case YYSYMBOL_counter_obj: /* counter_obj  */
#line 883 "src/parser_bison.y"
            { obj_free(((*yyvaluep).obj)); }
#line 7331 "src/parser_bison.c"
        break;

    case YYSYMBOL_quota_obj: /* quota_obj  */
#line 883 "src/parser_bison.y"
            { obj_free(((*yyvaluep).obj)); }
#line 7337 "src/parser_bison.c"
        break;

    case YYSYMBOL_secmark_obj: /* secmark_obj  */
#line 883 "src/parser_bison.y"
            { obj_free(((*yyvaluep).obj)); }
#line 7343 "src/parser_bison.c"
        break;

    case YYSYMBOL_timeout_states: /* timeout_states  */
#line 1003 "src/parser_bison.y"
            { timeout_states_free(((*yyvaluep).list)); }
#line 7349 "src/parser_bison.c"
        break;

    case YYSYMBOL_timeout_state: /* timeout_state  */
#line 1000 "src/parser_bison.y"
            { timeout_state_free(((*yyvaluep).timeout_state)); }
#line 7355 "src/parser_bison.c"
        break;

    case YYSYMBOL_ct_obj_alloc: /* ct_obj_alloc  */
#line 883 "src/parser_bison.y"
            { obj_free(((*yyvaluep).obj)); }
#line 7361 "src/parser_bison.c"
        break;

    case YYSYMBOL_limit_obj: /* limit_obj  */
#line 883 "src/parser_bison.y"
            { obj_free(((*yyvaluep).obj)); }
#line 7367 "src/parser_bison.c"
        break;

    case YYSYMBOL_relational_expr: /* relational_expr  */
#line 886 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7373 "src/parser_bison.c"
        break;

    case YYSYMBOL_list_rhs_expr: /* list_rhs_expr  */
#line 878 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7379 "src/parser_bison.c"
        break;

    case YYSYMBOL_rhs_expr: /* rhs_expr  */
#line 876 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7385 "src/parser_bison.c"
        break;

    case YYSYMBOL_shift_rhs_expr: /* shift_rhs_expr  */
#line 878 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7391 "src/parser_bison.c"
        break;

    case YYSYMBOL_and_rhs_expr: /* and_rhs_expr  */
#line 880 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7397 "src/parser_bison.c"
        break;

    case YYSYMBOL_exclusive_or_rhs_expr: /* exclusive_or_rhs_expr  */
#line 880 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7403 "src/parser_bison.c"
        break;

    case YYSYMBOL_inclusive_or_rhs_expr: /* inclusive_or_rhs_expr  */
#line 880 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7409 "src/parser_bison.c"
        break;

    case YYSYMBOL_basic_rhs_expr: /* basic_rhs_expr  */
#line 876 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7415 "src/parser_bison.c"
        break;

    case YYSYMBOL_concat_rhs_expr: /* concat_rhs_expr  */
#line 876 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7421 "src/parser_bison.c"
        break;

    case YYSYMBOL_boolean_expr: /* boolean_expr  */
#line 990 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7427 "src/parser_bison.c"
        break;

    case YYSYMBOL_keyword_expr: /* keyword_expr  */
#line 873 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7433 "src/parser_bison.c"
        break;

    case YYSYMBOL_primary_rhs_expr: /* primary_rhs_expr  */
#line 878 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7439 "src/parser_bison.c"
        break;

    case YYSYMBOL_verdict_expr: /* verdict_expr  */
#line 820 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7445 "src/parser_bison.c"
        break;

    case YYSYMBOL_chain_expr: /* chain_expr  */
#line 820 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7451 "src/parser_bison.c"
        break;

    case YYSYMBOL_meta_expr: /* meta_expr  */
#line 936 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7457 "src/parser_bison.c"
        break;

    case YYSYMBOL_meta_stmt: /* meta_stmt  */
#line 782 "src/parser_bison.y"
            { stmt_free(((*yyvaluep).stmt)); }
#line 7463 "src/parser_bison.c"
        break;

    case YYSYMBOL_socket_expr: /* socket_expr  */
#line 940 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7469 "src/parser_bison.c"
        break;

    case YYSYMBOL_numgen_expr: /* numgen_expr  */
#line 901 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7475 "src/parser_bison.c"
        break;

    case YYSYMBOL_xfrm_expr: /* xfrm_expr  */
#line 1007 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7481 "src/parser_bison.c"
        break;

    case YYSYMBOL_hash_expr: /* hash_expr  */
#line 901 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7487 "src/parser_bison.c"
        break;

    case YYSYMBOL_rt_expr: /* rt_expr  */
#line 946 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7493 "src/parser_bison.c"
        break;

    case YYSYMBOL_ct_expr: /* ct_expr  */
#line 950 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7499 "src/parser_bison.c"
        break;

    case YYSYMBOL_symbol_stmt_expr: /* symbol_stmt_expr  */
#line 878 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7505 "src/parser_bison.c"
        break;

    case YYSYMBOL_list_stmt_expr: /* list_stmt_expr  */
#line 846 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7511 "src/parser_bison.c"
        break;

    case YYSYMBOL_ct_stmt: /* ct_stmt  */
#line 780 "src/parser_bison.y"
            { stmt_free(((*yyvaluep).stmt)); }
#line 7517 "src/parser_bison.c"
        break;

    case YYSYMBOL_payload_stmt: /* payload_stmt  */
#line 778 "src/parser_bison.y"
            { stmt_free(((*yyvaluep).stmt)); }
#line 7523 "src/parser_bison.c"
        break;

    case YYSYMBOL_payload_expr: /* payload_expr  */
#line 890 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7529 "src/parser_bison.c"
        break;

    case YYSYMBOL_payload_raw_expr: /* payload_raw_expr  */
#line 890 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7535 "src/parser_bison.c"
        break;

    case YYSYMBOL_eth_hdr_expr: /* eth_hdr_expr  */
#line 895 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7541 "src/parser_bison.c"
        break;

    case YYSYMBOL_vlan_hdr_expr: /* vlan_hdr_expr  */
#line 895 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7547 "src/parser_bison.c"
        break;

    case YYSYMBOL_arp_hdr_expr: /* arp_hdr_expr  */
#line 898 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7553 "src/parser_bison.c"
        break;

    case YYSYMBOL_ip_hdr_expr: /* ip_hdr_expr  */
#line 901 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7559 "src/parser_bison.c"
        break;

    case YYSYMBOL_icmp_hdr_expr: /* icmp_hdr_expr  */
#line 901 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7565 "src/parser_bison.c"
        break;

    case YYSYMBOL_igmp_hdr_expr: /* igmp_hdr_expr  */
#line 901 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7571 "src/parser_bison.c"
        break;

    case YYSYMBOL_ip6_hdr_expr: /* ip6_hdr_expr  */
#line 905 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7577 "src/parser_bison.c"
        break;

    case YYSYMBOL_icmp6_hdr_expr: /* icmp6_hdr_expr  */
#line 905 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7583 "src/parser_bison.c"
        break;

    case YYSYMBOL_auth_hdr_expr: /* auth_hdr_expr  */
#line 908 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7589 "src/parser_bison.c"
        break;

    case YYSYMBOL_esp_hdr_expr: /* esp_hdr_expr  */
#line 908 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7595 "src/parser_bison.c"
        break;

    case YYSYMBOL_comp_hdr_expr: /* comp_hdr_expr  */
#line 908 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7601 "src/parser_bison.c"
        break;

    case YYSYMBOL_udp_hdr_expr: /* udp_hdr_expr  */
#line 911 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7607 "src/parser_bison.c"
        break;

    case YYSYMBOL_udplite_hdr_expr: /* udplite_hdr_expr  */
#line 911 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7613 "src/parser_bison.c"
        break;

    case YYSYMBOL_tcp_hdr_expr: /* tcp_hdr_expr  */
#line 969 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7619 "src/parser_bison.c"
        break;

    case YYSYMBOL_inner_inet_expr: /* inner_inet_expr  */
#line 977 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7625 "src/parser_bison.c"
        break;

    case YYSYMBOL_inner_eth_expr: /* inner_eth_expr  */
#line 977 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7631 "src/parser_bison.c"
        break;

    case YYSYMBOL_inner_expr: /* inner_expr  */
#line 977 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7637 "src/parser_bison.c"
        break;

    case YYSYMBOL_vxlan_hdr_expr: /* vxlan_hdr_expr  */
#line 980 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7643 "src/parser_bison.c"
        break;

    case YYSYMBOL_geneve_hdr_expr: /* geneve_hdr_expr  */
#line 980 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7649 "src/parser_bison.c"
        break;

    case YYSYMBOL_gre_hdr_expr: /* gre_hdr_expr  */
#line 980 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7655 "src/parser_bison.c"
        break;

    case YYSYMBOL_gretap_hdr_expr: /* gretap_hdr_expr  */
#line 980 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7661 "src/parser_bison.c"
        break;

    case YYSYMBOL_optstrip_stmt: /* optstrip_stmt  */
#line 984 "src/parser_bison.y"
            { stmt_free(((*yyvaluep).stmt)); }
#line 7667 "src/parser_bison.c"
        break;

    case YYSYMBOL_dccp_hdr_expr: /* dccp_hdr_expr  */
#line 914 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7673 "src/parser_bison.c"
        break;

    case YYSYMBOL_sctp_chunk_alloc: /* sctp_chunk_alloc  */
#line 914 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7679 "src/parser_bison.c"
        break;

    case YYSYMBOL_sctp_hdr_expr: /* sctp_hdr_expr  */
#line 914 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7685 "src/parser_bison.c"
        break;

    case YYSYMBOL_th_hdr_expr: /* th_hdr_expr  */
#line 920 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7691 "src/parser_bison.c"
        break;

    case YYSYMBOL_exthdr_expr: /* exthdr_expr  */
#line 924 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7697 "src/parser_bison.c"
        break;

    case YYSYMBOL_hbh_hdr_expr: /* hbh_hdr_expr  */
#line 926 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7703 "src/parser_bison.c"
        break;

    case YYSYMBOL_rt_hdr_expr: /* rt_hdr_expr  */
#line 929 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7709 "src/parser_bison.c"
        break;

    case YYSYMBOL_rt0_hdr_expr: /* rt0_hdr_expr  */
#line 929 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7715 "src/parser_bison.c"
        break;

    case YYSYMBOL_rt2_hdr_expr: /* rt2_hdr_expr  */
#line 929 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7721 "src/parser_bison.c"
        break;

    case YYSYMBOL_rt4_hdr_expr: /* rt4_hdr_expr  */
#line 929 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7727 "src/parser_bison.c"
        break;

    case YYSYMBOL_frag_hdr_expr: /* frag_hdr_expr  */
#line 926 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7733 "src/parser_bison.c"
        break;

    case YYSYMBOL_dst_hdr_expr: /* dst_hdr_expr  */
#line 926 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7739 "src/parser_bison.c"
        break;

    case YYSYMBOL_mh_hdr_expr: /* mh_hdr_expr  */
#line 932 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7745 "src/parser_bison.c"
        break;

    case YYSYMBOL_exthdr_exists_expr: /* exthdr_exists_expr  */
#line 994 "src/parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7751 "src/parser_bison.c"
        break;

      default:
        break;
    }
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}






/*----------.
| yyparse.  |
`----------*/

int
yyparse (struct nft_ctx *nft, void *scanner, struct parser_state *state)
{
/* Lookahead token kind.  */
int yychar;


/* The semantic value of the lookahead symbol.  */
/* Default value used for initialization, for pacifying older GCCs
   or non-GCC compilers.  */
YY_INITIAL_VALUE (static YYSTYPE yyval_default;)
YYSTYPE yylval YY_INITIAL_VALUE (= yyval_default);

/* Location data for the lookahead symbol.  */
static YYLTYPE yyloc_default
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL
  = { 1, 1, 1, 1 }
# endif
;
YYLTYPE yylloc = yyloc_default;

    /* Number of syntax errors so far.  */
    int yynerrs = 0;

    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

    /* The location stack: array, bottom, top.  */
    YYLTYPE yylsa[YYINITDEPTH];
    YYLTYPE *yyls = yylsa;
    YYLTYPE *yylsp = yyls;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;
  YYLTYPE yyloc;

  /* The locations where the error started and ended.  */
  YYLTYPE yyerror_range[3];

  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYPTRDIFF_T yymsg_alloc = sizeof yymsgbuf;

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N), yylsp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */


/* User initialization code.  */
#line 214 "src/parser_bison.y"
{
	location_init(scanner, state, &yylloc);
	if (nft->debug_mask & NFT_DEBUG_SCANNER)
		nft_set_debug(1, scanner);
	if (nft->debug_mask & NFT_DEBUG_PARSER)
		yydebug = 1;
}

#line 7857 "src/parser_bison.c"

  yylsp[0] = yylloc;
  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    YYNOMEM;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;
        YYLTYPE *yyls1 = yyls;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yyls1, yysize * YYSIZEOF (*yylsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
        yyls = yyls1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        YYNOMEM;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          YYNOMEM;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
        YYSTACK_RELOCATE (yyls_alloc, yyls);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;
      yylsp = yyls + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */


  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex (&yylval, &yylloc, scanner);
    }

  if (yychar <= TOKEN_EOF)
    {
      yychar = TOKEN_EOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      yyerror_range[1] = yylloc;
      goto yyerrlab1;
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END
  *++yylsp = yylloc;

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];

  /* Default location. */
  YYLLOC_DEFAULT (yyloc, (yylsp - yylen), yylen);
  yyerror_range[1] = yyloc;
  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 3: /* input: input line  */
#line 1016 "src/parser_bison.y"
                        {
				if ((yyvsp[0].cmd) != NULL) {
					(yyvsp[0].cmd)->location = (yylsp[0]);
					list_add_tail(&(yyvsp[0].cmd)->list, state->cmds);
				}
			}
#line 8075 "src/parser_bison.c"
    break;

  case 8: /* close_scope_ah: %empty  */
#line 1032 "src/parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_EXPR_AH); }
#line 8081 "src/parser_bison.c"
    break;

  case 9: /* close_scope_arp: %empty  */
#line 1033 "src/parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_ARP); }
#line 8087 "src/parser_bison.c"
    break;

  case 10: /* close_scope_at: %empty  */
#line 1034 "src/parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_AT); }
#line 8093 "src/parser_bison.c"
    break;

  case 11: /* close_scope_comp: %empty  */
#line 1035 "src/parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_EXPR_COMP); }
#line 8099 "src/parser_bison.c"
    break;

  case 12: /* close_scope_ct: %empty  */
#line 1036 "src/parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_CT); }
#line 8105 "src/parser_bison.c"
    break;

  case 13: /* close_scope_counter: %empty  */
#line 1037 "src/parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_COUNTER); }
#line 8111 "src/parser_bison.c"
    break;

  case 14: /* close_scope_last: %empty  */
#line 1038 "src/parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_LAST); }
#line 8117 "src/parser_bison.c"
    break;

  case 15: /* close_scope_dccp: %empty  */
#line 1039 "src/parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_EXPR_DCCP); }
#line 8123 "src/parser_bison.c"
    break;

  case 16: /* close_scope_destroy: %empty  */
#line 1040 "src/parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_CMD_DESTROY); }
#line 8129 "src/parser_bison.c"
    break;

  case 17: /* close_scope_dst: %empty  */
#line 1041 "src/parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_EXPR_DST); }
#line 8135 "src/parser_bison.c"
    break;

  case 18: /* close_scope_dup: %empty  */
#line 1042 "src/parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_STMT_DUP); }
#line 8141 "src/parser_bison.c"
    break;

  case 19: /* close_scope_esp: %empty  */
#line 1043 "src/parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_EXPR_ESP); }
#line 8147 "src/parser_bison.c"
    break;

  case 20: /* close_scope_eth: %empty  */
#line 1044 "src/parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_ETH); }
#line 8153 "src/parser_bison.c"
    break;

  case 21: /* close_scope_export: %empty  */
#line 1045 "src/parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_CMD_EXPORT); }
#line 8159 "src/parser_bison.c"
    break;

  case 22: /* close_scope_fib: %empty  */
#line 1046 "src/parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_EXPR_FIB); }
#line 8165 "src/parser_bison.c"
    break;

  case 23: /* close_scope_frag: %empty  */
#line 1047 "src/parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_EXPR_FRAG); }
#line 8171 "src/parser_bison.c"
    break;

  case 24: /* close_scope_fwd: %empty  */
#line 1048 "src/parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_STMT_FWD); }
#line 8177 "src/parser_bison.c"
    break;

  case 25: /* close_scope_gre: %empty  */
#line 1049 "src/parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_GRE); }
#line 8183 "src/parser_bison.c"
    break;

  case 26: /* close_scope_hash: %empty  */
#line 1050 "src/parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_EXPR_HASH); }
#line 8189 "src/parser_bison.c"
    break;

  case 27: /* close_scope_hbh: %empty  */
#line 1051 "src/parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_EXPR_HBH); }
#line 8195 "src/parser_bison.c"
    break;

  case 28: /* close_scope_ip: %empty  */
#line 1052 "src/parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_IP); }
#line 8201 "src/parser_bison.c"
    break;

  case 29: /* close_scope_ip6: %empty  */
#line 1053 "src/parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_IP6); }
#line 8207 "src/parser_bison.c"
    break;

  case 30: /* close_scope_vlan: %empty  */
#line 1054 "src/parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_VLAN); }
#line 8213 "src/parser_bison.c"
    break;

  case 31: /* close_scope_icmp: %empty  */
#line 1055 "src/parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_ICMP); }
#line 8219 "src/parser_bison.c"
    break;

  case 32: /* close_scope_igmp: %empty  */
#line 1056 "src/parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_IGMP); }
#line 8225 "src/parser_bison.c"
    break;

  case 33: /* close_scope_import: %empty  */
#line 1057 "src/parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_CMD_IMPORT); }
#line 8231 "src/parser_bison.c"
    break;

  case 34: /* close_scope_ipsec: %empty  */
#line 1058 "src/parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_EXPR_IPSEC); }
#line 8237 "src/parser_bison.c"
    break;

  case 35: /* close_scope_list: %empty  */
#line 1059 "src/parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_CMD_LIST); }
#line 8243 "src/parser_bison.c"
    break;

  case 36: /* close_scope_limit: %empty  */
#line 1060 "src/parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_LIMIT); }
#line 8249 "src/parser_bison.c"
    break;

  case 37: /* close_scope_meta: %empty  */
#line 1061 "src/parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_META); }
#line 8255 "src/parser_bison.c"
    break;

  case 38: /* close_scope_mh: %empty  */
#line 1062 "src/parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_EXPR_MH); }
#line 8261 "src/parser_bison.c"
    break;

  case 39: /* close_scope_monitor: %empty  */
#line 1063 "src/parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_CMD_MONITOR); }
#line 8267 "src/parser_bison.c"
    break;

  case 40: /* close_scope_nat: %empty  */
#line 1064 "src/parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_STMT_NAT); }
#line 8273 "src/parser_bison.c"
    break;

  case 41: /* close_scope_numgen: %empty  */
#line 1065 "src/parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_EXPR_NUMGEN); }
#line 8279 "src/parser_bison.c"
    break;

  case 42: /* close_scope_osf: %empty  */
#line 1066 "src/parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_EXPR_OSF); }
#line 8285 "src/parser_bison.c"
    break;

  case 43: /* close_scope_policy: %empty  */
#line 1067 "src/parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_POLICY); }
#line 8291 "src/parser_bison.c"
    break;

  case 44: /* close_scope_quota: %empty  */
#line 1068 "src/parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_QUOTA); }
#line 8297 "src/parser_bison.c"
    break;

  case 45: /* close_scope_queue: %empty  */
#line 1069 "src/parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_EXPR_QUEUE); }
#line 8303 "src/parser_bison.c"
    break;

  case 46: /* close_scope_reject: %empty  */
#line 1070 "src/parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_STMT_REJECT); }
#line 8309 "src/parser_bison.c"
    break;

  case 47: /* close_scope_reset: %empty  */
#line 1071 "src/parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_CMD_RESET); }
#line 8315 "src/parser_bison.c"
    break;

  case 48: /* close_scope_rt: %empty  */
#line 1072 "src/parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_EXPR_RT); }
#line 8321 "src/parser_bison.c"
    break;

  case 49: /* close_scope_sctp: %empty  */
#line 1073 "src/parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_SCTP); }
#line 8327 "src/parser_bison.c"
    break;

  case 50: /* close_scope_sctp_chunk: %empty  */
#line 1074 "src/parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_EXPR_SCTP_CHUNK); }
#line 8333 "src/parser_bison.c"
    break;

  case 51: /* close_scope_secmark: %empty  */
#line 1075 "src/parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_SECMARK); }
#line 8339 "src/parser_bison.c"
    break;

  case 52: /* close_scope_socket: %empty  */
#line 1076 "src/parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_EXPR_SOCKET); }
#line 8345 "src/parser_bison.c"
    break;

  case 53: /* close_scope_tcp: %empty  */
#line 1077 "src/parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_TCP); }
#line 8351 "src/parser_bison.c"
    break;

  case 54: /* close_scope_tproxy: %empty  */
#line 1078 "src/parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_STMT_TPROXY); }
#line 8357 "src/parser_bison.c"
    break;

  case 55: /* close_scope_type: %empty  */
#line 1079 "src/parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_TYPE); }
#line 8363 "src/parser_bison.c"
    break;

  case 56: /* close_scope_th: %empty  */
#line 1080 "src/parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_EXPR_TH); }
#line 8369 "src/parser_bison.c"
    break;

  case 57: /* close_scope_udp: %empty  */
#line 1081 "src/parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_EXPR_UDP); }
#line 8375 "src/parser_bison.c"
    break;

  case 58: /* close_scope_udplite: %empty  */
#line 1082 "src/parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_EXPR_UDPLITE); }
#line 8381 "src/parser_bison.c"
    break;

  case 59: /* close_scope_log: %empty  */
#line 1084 "src/parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_STMT_LOG); }
#line 8387 "src/parser_bison.c"
    break;

  case 60: /* close_scope_synproxy: %empty  */
#line 1085 "src/parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_STMT_SYNPROXY); }
#line 8393 "src/parser_bison.c"
    break;

  case 61: /* close_scope_xt: %empty  */
#line 1086 "src/parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_XT); }
#line 8399 "src/parser_bison.c"
    break;

  case 62: /* common_block: "include" "quoted string" stmt_separator  */
#line 1089 "src/parser_bison.y"
                        {
				if (scanner_include_file(nft, scanner, (yyvsp[-1].string), &(yyloc)) < 0) {
					free_const((yyvsp[-1].string));
					YYERROR;
				}
				free_const((yyvsp[-1].string));
			}
#line 8411 "src/parser_bison.c"
    break;

  case 63: /* common_block: "define" identifier '=' initializer_expr stmt_separator  */
#line 1097 "src/parser_bison.y"
                        {
				struct scope *scope = current_scope(state);

				if (symbol_lookup(scope, (yyvsp[-3].string)) != NULL) {
					erec_queue(error(&(yylsp[-3]), "redefinition of symbol '%s'", (yyvsp[-3].string)),
						   state->msgs);
					expr_free((yyvsp[-1].expr));
					free_const((yyvsp[-3].string));
					YYERROR;
				}

				symbol_bind(scope, (yyvsp[-3].string), (yyvsp[-1].expr));
				free_const((yyvsp[-3].string));
			}
#line 8430 "src/parser_bison.c"
    break;

  case 64: /* common_block: "redefine" identifier '=' initializer_expr stmt_separator  */
#line 1112 "src/parser_bison.y"
                        {
				struct scope *scope = current_scope(state);

				symbol_bind(scope, (yyvsp[-3].string), (yyvsp[-1].expr));
				free_const((yyvsp[-3].string));
			}
#line 8441 "src/parser_bison.c"
    break;

  case 65: /* common_block: "undefine" identifier stmt_separator  */
#line 1119 "src/parser_bison.y"
                        {
				struct scope *scope = current_scope(state);

				if (symbol_unbind(scope, (yyvsp[-1].string)) < 0) {
					erec_queue(error(&(yylsp[-1]), "undefined symbol '%s'", (yyvsp[-1].string)),
						   state->msgs);
					free_const((yyvsp[-1].string));
					YYERROR;
				}
				free_const((yyvsp[-1].string));
			}
#line 8457 "src/parser_bison.c"
    break;

  case 66: /* common_block: error stmt_separator  */
#line 1131 "src/parser_bison.y"
                        {
				if (++state->nerrs == nft->parser_max_errors)
					YYABORT;
				yyerrok;
			}
#line 8467 "src/parser_bison.c"
    break;

  case 67: /* line: common_block  */
#line 1138 "src/parser_bison.y"
                                                                { (yyval.cmd) = NULL; }
#line 8473 "src/parser_bison.c"
    break;

  case 68: /* line: stmt_separator  */
#line 1139 "src/parser_bison.y"
                                                                { (yyval.cmd) = NULL; }
#line 8479 "src/parser_bison.c"
    break;

  case 69: /* line: base_cmd stmt_separator  */
#line 1140 "src/parser_bison.y"
                                                                { (yyval.cmd) = (yyvsp[-1].cmd); }
#line 8485 "src/parser_bison.c"
    break;

  case 70: /* line: base_cmd "end of file"  */
#line 1142 "src/parser_bison.y"
                        {
				/*
				 * Very hackish workaround for bison >= 2.4: previous versions
				 * terminated parsing after EOF, 2.4+ tries to get further input
				 * in 'input' and calls the scanner again, causing a crash when
				 * the final input buffer has been popped. Terminate manually to
				 * avoid this. The correct fix should be to adjust the grammar
				 * to accept EOF in input, but for unknown reasons it does not
				 * work.
				 */
				if ((yyvsp[-1].cmd) != NULL) {
					(yyvsp[-1].cmd)->location = (yylsp[-1]);
					list_add_tail(&(yyvsp[-1].cmd)->list, state->cmds);
				}
				(yyval.cmd) = NULL;
				YYACCEPT;
			}
#line 8507 "src/parser_bison.c"
    break;

  case 71: /* base_cmd: add_cmd  */
#line 1161 "src/parser_bison.y"
                                                                { (yyval.cmd) = (yyvsp[0].cmd); }
#line 8513 "src/parser_bison.c"
    break;

  case 72: /* base_cmd: "add" add_cmd  */
#line 1162 "src/parser_bison.y"
                                                                { (yyval.cmd) = (yyvsp[0].cmd); }
#line 8519 "src/parser_bison.c"
    break;

  case 73: /* base_cmd: "replace" replace_cmd  */
#line 1163 "src/parser_bison.y"
                                                                { (yyval.cmd) = (yyvsp[0].cmd); }
#line 8525 "src/parser_bison.c"
    break;

  case 74: /* base_cmd: "create" create_cmd  */
#line 1164 "src/parser_bison.y"
                                                                { (yyval.cmd) = (yyvsp[0].cmd); }
#line 8531 "src/parser_bison.c"
    break;

  case 75: /* base_cmd: "insert" insert_cmd  */
#line 1165 "src/parser_bison.y"
                                                                { (yyval.cmd) = (yyvsp[0].cmd); }
#line 8537 "src/parser_bison.c"
    break;

  case 76: /* base_cmd: "delete" delete_cmd  */
#line 1166 "src/parser_bison.y"
                                                                { (yyval.cmd) = (yyvsp[0].cmd); }
#line 8543 "src/parser_bison.c"
    break;

  case 77: /* base_cmd: "get" get_cmd  */
#line 1167 "src/parser_bison.y"
                                                                { (yyval.cmd) = (yyvsp[0].cmd); }
#line 8549 "src/parser_bison.c"
    break;

  case 78: /* base_cmd: "list" list_cmd close_scope_list  */
#line 1168 "src/parser_bison.y"
                                                                                        { (yyval.cmd) = (yyvsp[-1].cmd); }
#line 8555 "src/parser_bison.c"
    break;

  case 79: /* base_cmd: "reset" reset_cmd close_scope_reset  */
#line 1169 "src/parser_bison.y"
                                                                                        { (yyval.cmd) = (yyvsp[-1].cmd); }
#line 8561 "src/parser_bison.c"
    break;

  case 80: /* base_cmd: "flush" flush_cmd  */
#line 1170 "src/parser_bison.y"
                                                                { (yyval.cmd) = (yyvsp[0].cmd); }
#line 8567 "src/parser_bison.c"
    break;

  case 81: /* base_cmd: "rename" rename_cmd  */
#line 1171 "src/parser_bison.y"
                                                                { (yyval.cmd) = (yyvsp[0].cmd); }
#line 8573 "src/parser_bison.c"
    break;

  case 82: /* base_cmd: "import" import_cmd close_scope_import  */
#line 1172 "src/parser_bison.y"
                                                                                        { (yyval.cmd) = (yyvsp[-1].cmd); }
#line 8579 "src/parser_bison.c"
    break;

  case 83: /* base_cmd: "export" export_cmd close_scope_export  */
#line 1173 "src/parser_bison.y"
                                                                                        { (yyval.cmd) = (yyvsp[-1].cmd); }
#line 8585 "src/parser_bison.c"
    break;

  case 84: /* base_cmd: "monitor" monitor_cmd close_scope_monitor  */
#line 1174 "src/parser_bison.y"
                                                                                        { (yyval.cmd) = (yyvsp[-1].cmd); }
#line 8591 "src/parser_bison.c"
    break;

  case 85: /* base_cmd: "describe" describe_cmd  */
#line 1175 "src/parser_bison.y"
                                                                { (yyval.cmd) = (yyvsp[0].cmd); }
#line 8597 "src/parser_bison.c"
    break;

  case 86: /* base_cmd: "destroy" destroy_cmd close_scope_destroy  */
#line 1176 "src/parser_bison.y"
                                                                                        { (yyval.cmd) = (yyvsp[-1].cmd); }
#line 8603 "src/parser_bison.c"
    break;

  case 87: /* add_cmd: "table" table_spec  */
#line 1180 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_ADD, CMD_OBJ_TABLE, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 8611 "src/parser_bison.c"
    break;

  case 88: /* add_cmd: "table" table_spec table_block_alloc '{' table_block '}'  */
#line 1185 "src/parser_bison.y"
                        {
				handle_merge(&(yyvsp[-3].table)->handle, &(yyvsp[-4].handle));
				close_scope(state);
				(yyval.cmd) = cmd_alloc(CMD_ADD, CMD_OBJ_TABLE, &(yyvsp[-4].handle), &(yyloc), (yyvsp[-1].table));
			}
#line 8621 "src/parser_bison.c"
    break;

  case 89: /* add_cmd: "chain" chain_spec  */
#line 1191 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_ADD, CMD_OBJ_CHAIN, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 8629 "src/parser_bison.c"
    break;

  case 90: /* add_cmd: "chain" chain_spec chain_block_alloc '{' chain_block '}'  */
#line 1196 "src/parser_bison.y"
                        {
				(yyvsp[-1].chain)->location = (yylsp[-1]);
				handle_merge(&(yyvsp[-3].chain)->handle, &(yyvsp[-4].handle));
				close_scope(state);
				(yyval.cmd) = cmd_alloc(CMD_ADD, CMD_OBJ_CHAIN, &(yyvsp[-4].handle), &(yyloc), (yyvsp[-1].chain));
			}
#line 8640 "src/parser_bison.c"
    break;

  case 91: /* add_cmd: "rule" rule_position rule  */
#line 1203 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_ADD, CMD_OBJ_RULE, &(yyvsp[-1].handle), &(yyloc), (yyvsp[0].rule));
			}
#line 8648 "src/parser_bison.c"
    break;

  case 92: /* add_cmd: rule_position rule  */
#line 1207 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_ADD, CMD_OBJ_RULE, &(yyvsp[-1].handle), &(yyloc), (yyvsp[0].rule));
			}
#line 8656 "src/parser_bison.c"
    break;

  case 93: /* add_cmd: "set" set_spec set_block_alloc '{' set_block '}'  */
#line 1212 "src/parser_bison.y"
                        {
				(yyvsp[-1].set)->location = (yylsp[-1]);
				handle_merge(&(yyvsp[-3].set)->handle, &(yyvsp[-4].handle));
				(yyval.cmd) = cmd_alloc(CMD_ADD, CMD_OBJ_SET, &(yyvsp[-4].handle), &(yyloc), (yyvsp[-1].set));
			}
#line 8666 "src/parser_bison.c"
    break;

  case 94: /* add_cmd: "map" set_spec map_block_alloc '{' map_block '}'  */
#line 1219 "src/parser_bison.y"
                        {
				(yyvsp[-1].set)->location = (yylsp[-1]);
				handle_merge(&(yyvsp[-3].set)->handle, &(yyvsp[-4].handle));
				(yyval.cmd) = cmd_alloc(CMD_ADD, CMD_OBJ_SET, &(yyvsp[-4].handle), &(yyloc), (yyvsp[-1].set));
			}
#line 8676 "src/parser_bison.c"
    break;

  case 95: /* add_cmd: "element" set_spec set_block_expr  */
#line 1225 "src/parser_bison.y"
                        {
				if (nft_cmd_collapse_elems(CMD_ADD, state->cmds, &(yyvsp[-1].handle), (yyvsp[0].expr))) {
					handle_free(&(yyvsp[-1].handle));
					expr_free((yyvsp[0].expr));
					(yyval.cmd) = NULL;
					break;
				}
				(yyval.cmd) = cmd_alloc(CMD_ADD, CMD_OBJ_ELEMENTS, &(yyvsp[-1].handle), &(yyloc), (yyvsp[0].expr));
			}
#line 8690 "src/parser_bison.c"
    break;

  case 96: /* add_cmd: "flowtable" flowtable_spec flowtable_block_alloc '{' flowtable_block '}'  */
#line 1236 "src/parser_bison.y"
                        {
				(yyvsp[-1].flowtable)->location = (yylsp[-1]);
				handle_merge(&(yyvsp[-3].flowtable)->handle, &(yyvsp[-4].handle));
				(yyval.cmd) = cmd_alloc(CMD_ADD, CMD_OBJ_FLOWTABLE, &(yyvsp[-4].handle), &(yyloc), (yyvsp[-1].flowtable));
			}
#line 8700 "src/parser_bison.c"
    break;

  case 97: /* add_cmd: "counter" obj_spec close_scope_counter  */
#line 1242 "src/parser_bison.y"
                        {
				struct obj *obj;

				obj = obj_alloc(&(yyloc));
				obj->type = NFT_OBJECT_COUNTER;
				handle_merge(&obj->handle, &(yyvsp[-1].handle));
				(yyval.cmd) = cmd_alloc(CMD_ADD, CMD_OBJ_COUNTER, &(yyvsp[-1].handle), &(yyloc), obj);
			}
#line 8713 "src/parser_bison.c"
    break;

  case 98: /* add_cmd: "counter" obj_spec counter_obj counter_config close_scope_counter  */
#line 1251 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_ADD, CMD_OBJ_COUNTER, &(yyvsp[-3].handle), &(yyloc), (yyvsp[-2].obj));
			}
#line 8721 "src/parser_bison.c"
    break;

  case 99: /* add_cmd: "counter" obj_spec counter_obj '{' counter_block '}' close_scope_counter  */
#line 1255 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_ADD, CMD_OBJ_COUNTER, &(yyvsp[-5].handle), &(yyloc), (yyvsp[-4].obj));
			}
#line 8729 "src/parser_bison.c"
    break;

  case 100: /* add_cmd: "quota" obj_spec quota_obj quota_config close_scope_quota  */
#line 1259 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_ADD, CMD_OBJ_QUOTA, &(yyvsp[-3].handle), &(yyloc), (yyvsp[-2].obj));
			}
#line 8737 "src/parser_bison.c"
    break;

  case 101: /* add_cmd: "quota" obj_spec quota_obj '{' quota_block '}' close_scope_quota  */
#line 1263 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_ADD, CMD_OBJ_QUOTA, &(yyvsp[-5].handle), &(yyloc), (yyvsp[-4].obj));
			}
#line 8745 "src/parser_bison.c"
    break;

  case 102: /* add_cmd: "ct" "helper" obj_spec ct_obj_alloc '{' ct_helper_block '}' close_scope_ct  */
#line 1267 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc_obj_ct(CMD_ADD, NFT_OBJECT_CT_HELPER, &(yyvsp[-5].handle), &(yyloc), (yyvsp[-4].obj));
			}
#line 8753 "src/parser_bison.c"
    break;

  case 103: /* add_cmd: "ct" "timeout" obj_spec ct_obj_alloc '{' ct_timeout_block '}' close_scope_ct  */
#line 1271 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc_obj_ct(CMD_ADD, NFT_OBJECT_CT_TIMEOUT, &(yyvsp[-5].handle), &(yyloc), (yyvsp[-4].obj));
			}
#line 8761 "src/parser_bison.c"
    break;

  case 104: /* add_cmd: "ct" "expectation" obj_spec ct_obj_alloc '{' ct_expect_block '}' close_scope_ct  */
#line 1275 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc_obj_ct(CMD_ADD, NFT_OBJECT_CT_EXPECT, &(yyvsp[-5].handle), &(yyloc), (yyvsp[-4].obj));
			}
#line 8769 "src/parser_bison.c"
    break;

  case 105: /* add_cmd: "limit" obj_spec limit_obj limit_config close_scope_limit  */
#line 1279 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_ADD, CMD_OBJ_LIMIT, &(yyvsp[-3].handle), &(yyloc), (yyvsp[-2].obj));
			}
#line 8777 "src/parser_bison.c"
    break;

  case 106: /* add_cmd: "limit" obj_spec limit_obj '{' limit_block '}' close_scope_limit  */
#line 1283 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_ADD, CMD_OBJ_LIMIT, &(yyvsp[-5].handle), &(yyloc), (yyvsp[-4].obj));
			}
#line 8785 "src/parser_bison.c"
    break;

  case 107: /* add_cmd: "secmark" obj_spec secmark_obj secmark_config close_scope_secmark  */
#line 1287 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_ADD, CMD_OBJ_SECMARK, &(yyvsp[-3].handle), &(yyloc), (yyvsp[-2].obj));
			}
#line 8793 "src/parser_bison.c"
    break;

  case 108: /* add_cmd: "secmark" obj_spec secmark_obj '{' secmark_block '}' close_scope_secmark  */
#line 1291 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_ADD, CMD_OBJ_SECMARK, &(yyvsp[-5].handle), &(yyloc), (yyvsp[-4].obj));
			}
#line 8801 "src/parser_bison.c"
    break;

  case 109: /* add_cmd: "synproxy" obj_spec synproxy_obj synproxy_config close_scope_synproxy  */
#line 1295 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_ADD, CMD_OBJ_SYNPROXY, &(yyvsp[-3].handle), &(yyloc), (yyvsp[-2].obj));
			}
#line 8809 "src/parser_bison.c"
    break;

  case 110: /* add_cmd: "synproxy" obj_spec synproxy_obj '{' synproxy_block '}' close_scope_synproxy  */
#line 1299 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_ADD, CMD_OBJ_SYNPROXY, &(yyvsp[-5].handle), &(yyloc), (yyvsp[-4].obj));
			}
#line 8817 "src/parser_bison.c"
    break;

  case 111: /* replace_cmd: "rule" ruleid_spec rule  */
#line 1305 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_REPLACE, CMD_OBJ_RULE, &(yyvsp[-1].handle), &(yyloc), (yyvsp[0].rule));
			}
#line 8825 "src/parser_bison.c"
    break;

  case 112: /* create_cmd: "table" table_spec  */
#line 1311 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_CREATE, CMD_OBJ_TABLE, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 8833 "src/parser_bison.c"
    break;

  case 113: /* create_cmd: "table" table_spec table_block_alloc '{' table_block '}'  */
#line 1316 "src/parser_bison.y"
                        {
				handle_merge(&(yyvsp[-3].table)->handle, &(yyvsp[-4].handle));
				close_scope(state);
				(yyval.cmd) = cmd_alloc(CMD_CREATE, CMD_OBJ_TABLE, &(yyvsp[-4].handle), &(yyloc), (yyvsp[-1].table));
			}
#line 8843 "src/parser_bison.c"
    break;

  case 114: /* create_cmd: "chain" chain_spec  */
#line 1322 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_CREATE, CMD_OBJ_CHAIN, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 8851 "src/parser_bison.c"
    break;

  case 115: /* create_cmd: "chain" chain_spec chain_block_alloc '{' chain_block '}'  */
#line 1327 "src/parser_bison.y"
                        {
				(yyvsp[-1].chain)->location = (yylsp[-1]);
				handle_merge(&(yyvsp[-3].chain)->handle, &(yyvsp[-4].handle));
				close_scope(state);
				(yyval.cmd) = cmd_alloc(CMD_CREATE, CMD_OBJ_CHAIN, &(yyvsp[-4].handle), &(yyloc), (yyvsp[-1].chain));
			}
#line 8862 "src/parser_bison.c"
    break;

  case 116: /* create_cmd: "set" set_spec set_block_alloc '{' set_block '}'  */
#line 1335 "src/parser_bison.y"
                        {
				(yyvsp[-1].set)->location = (yylsp[-1]);
				handle_merge(&(yyvsp[-3].set)->handle, &(yyvsp[-4].handle));
				(yyval.cmd) = cmd_alloc(CMD_CREATE, CMD_OBJ_SET, &(yyvsp[-4].handle), &(yyloc), (yyvsp[-1].set));
			}
#line 8872 "src/parser_bison.c"
    break;

  case 117: /* create_cmd: "map" set_spec map_block_alloc '{' map_block '}'  */
#line 1342 "src/parser_bison.y"
                        {
				(yyvsp[-1].set)->location = (yylsp[-1]);
				handle_merge(&(yyvsp[-3].set)->handle, &(yyvsp[-4].handle));
				(yyval.cmd) = cmd_alloc(CMD_CREATE, CMD_OBJ_SET, &(yyvsp[-4].handle), &(yyloc), (yyvsp[-1].set));
			}
#line 8882 "src/parser_bison.c"
    break;

  case 118: /* create_cmd: "element" set_spec set_block_expr  */
#line 1348 "src/parser_bison.y"
                        {
				if (nft_cmd_collapse_elems(CMD_CREATE, state->cmds, &(yyvsp[-1].handle), (yyvsp[0].expr))) {
					handle_free(&(yyvsp[-1].handle));
					expr_free((yyvsp[0].expr));
					(yyval.cmd) = NULL;
					break;
				}
				(yyval.cmd) = cmd_alloc(CMD_CREATE, CMD_OBJ_ELEMENTS, &(yyvsp[-1].handle), &(yyloc), (yyvsp[0].expr));
			}
#line 8896 "src/parser_bison.c"
    break;

  case 119: /* create_cmd: "flowtable" flowtable_spec flowtable_block_alloc '{' flowtable_block '}'  */
#line 1359 "src/parser_bison.y"
                        {
				(yyvsp[-1].flowtable)->location = (yylsp[-1]);
				handle_merge(&(yyvsp[-3].flowtable)->handle, &(yyvsp[-4].handle));
				(yyval.cmd) = cmd_alloc(CMD_CREATE, CMD_OBJ_FLOWTABLE, &(yyvsp[-4].handle), &(yyloc), (yyvsp[-1].flowtable));
			}
#line 8906 "src/parser_bison.c"
    break;

  case 120: /* create_cmd: "counter" obj_spec close_scope_counter  */
#line 1365 "src/parser_bison.y"
                        {
				struct obj *obj;

				obj = obj_alloc(&(yyloc));
				obj->type = NFT_OBJECT_COUNTER;
				handle_merge(&obj->handle, &(yyvsp[-1].handle));
				(yyval.cmd) = cmd_alloc(CMD_CREATE, CMD_OBJ_COUNTER, &(yyvsp[-1].handle), &(yyloc), obj);
			}
#line 8919 "src/parser_bison.c"
    break;

  case 121: /* create_cmd: "counter" obj_spec counter_obj counter_config close_scope_counter  */
#line 1374 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_CREATE, CMD_OBJ_COUNTER, &(yyvsp[-3].handle), &(yyloc), (yyvsp[-2].obj));
			}
#line 8927 "src/parser_bison.c"
    break;

  case 122: /* create_cmd: "quota" obj_spec quota_obj quota_config close_scope_quota  */
#line 1378 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_CREATE, CMD_OBJ_QUOTA, &(yyvsp[-3].handle), &(yyloc), (yyvsp[-2].obj));
			}
#line 8935 "src/parser_bison.c"
    break;

  case 123: /* create_cmd: "ct" "helper" obj_spec ct_obj_alloc '{' ct_helper_block '}' close_scope_ct  */
#line 1382 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc_obj_ct(CMD_CREATE, NFT_OBJECT_CT_HELPER, &(yyvsp[-5].handle), &(yyloc), (yyvsp[-4].obj));
			}
#line 8943 "src/parser_bison.c"
    break;

  case 124: /* create_cmd: "ct" "timeout" obj_spec ct_obj_alloc '{' ct_timeout_block '}' close_scope_ct  */
#line 1386 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc_obj_ct(CMD_CREATE, NFT_OBJECT_CT_TIMEOUT, &(yyvsp[-5].handle), &(yyloc), (yyvsp[-4].obj));
			}
#line 8951 "src/parser_bison.c"
    break;

  case 125: /* create_cmd: "ct" "expectation" obj_spec ct_obj_alloc '{' ct_expect_block '}' close_scope_ct  */
#line 1390 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc_obj_ct(CMD_CREATE, NFT_OBJECT_CT_EXPECT, &(yyvsp[-5].handle), &(yyloc), (yyvsp[-4].obj));
			}
#line 8959 "src/parser_bison.c"
    break;

  case 126: /* create_cmd: "limit" obj_spec limit_obj limit_config close_scope_limit  */
#line 1394 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_CREATE, CMD_OBJ_LIMIT, &(yyvsp[-3].handle), &(yyloc), (yyvsp[-2].obj));
			}
#line 8967 "src/parser_bison.c"
    break;

  case 127: /* create_cmd: "secmark" obj_spec secmark_obj secmark_config close_scope_secmark  */
#line 1398 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_CREATE, CMD_OBJ_SECMARK, &(yyvsp[-3].handle), &(yyloc), (yyvsp[-2].obj));
			}
#line 8975 "src/parser_bison.c"
    break;

  case 128: /* create_cmd: "synproxy" obj_spec synproxy_obj synproxy_config close_scope_synproxy  */
#line 1402 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_CREATE, CMD_OBJ_SYNPROXY, &(yyvsp[-3].handle), &(yyloc), (yyvsp[-2].obj));
			}
#line 8983 "src/parser_bison.c"
    break;

  case 129: /* insert_cmd: "rule" rule_position rule  */
#line 1408 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_INSERT, CMD_OBJ_RULE, &(yyvsp[-1].handle), &(yyloc), (yyvsp[0].rule));
			}
#line 8991 "src/parser_bison.c"
    break;

  case 138: /* delete_cmd: "table" table_or_id_spec  */
#line 1430 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_DELETE, CMD_OBJ_TABLE, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 8999 "src/parser_bison.c"
    break;

  case 139: /* delete_cmd: "chain" chain_or_id_spec  */
#line 1434 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_DELETE, CMD_OBJ_CHAIN, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 9007 "src/parser_bison.c"
    break;

  case 140: /* delete_cmd: "chain" chain_spec chain_block_alloc '{' chain_block '}'  */
#line 1439 "src/parser_bison.y"
                        {
				(yyvsp[-1].chain)->location = (yylsp[-1]);
				handle_merge(&(yyvsp[-3].chain)->handle, &(yyvsp[-4].handle));
				close_scope(state);
				(yyval.cmd) = cmd_alloc(CMD_DELETE, CMD_OBJ_CHAIN, &(yyvsp[-4].handle), &(yyloc), (yyvsp[-1].chain));
			}
#line 9018 "src/parser_bison.c"
    break;

  case 141: /* delete_cmd: "rule" ruleid_spec  */
#line 1446 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_DELETE, CMD_OBJ_RULE, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 9026 "src/parser_bison.c"
    break;

  case 142: /* delete_cmd: "set" set_or_id_spec  */
#line 1450 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_DELETE, CMD_OBJ_SET, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 9034 "src/parser_bison.c"
    break;

  case 143: /* delete_cmd: "map" set_spec  */
#line 1454 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_DELETE, CMD_OBJ_SET, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 9042 "src/parser_bison.c"
    break;

  case 144: /* delete_cmd: "element" set_spec set_block_expr  */
#line 1458 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_DELETE, CMD_OBJ_ELEMENTS, &(yyvsp[-1].handle), &(yyloc), (yyvsp[0].expr));
			}
#line 9050 "src/parser_bison.c"
    break;

  case 145: /* delete_cmd: "flowtable" flowtable_spec  */
#line 1462 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_DELETE, CMD_OBJ_FLOWTABLE, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 9058 "src/parser_bison.c"
    break;

  case 146: /* delete_cmd: "flowtable" flowtableid_spec  */
#line 1466 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_DELETE, CMD_OBJ_FLOWTABLE, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 9066 "src/parser_bison.c"
    break;

  case 147: /* delete_cmd: "flowtable" flowtable_spec flowtable_block_alloc '{' flowtable_block '}'  */
#line 1471 "src/parser_bison.y"
                        {
				(yyvsp[-1].flowtable)->location = (yylsp[-1]);
				handle_merge(&(yyvsp[-3].flowtable)->handle, &(yyvsp[-4].handle));
				(yyval.cmd) = cmd_alloc(CMD_DELETE, CMD_OBJ_FLOWTABLE, &(yyvsp[-4].handle), &(yyloc), (yyvsp[-1].flowtable));
			}
#line 9076 "src/parser_bison.c"
    break;

  case 148: /* delete_cmd: "counter" obj_or_id_spec close_scope_counter  */
#line 1477 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_DELETE, CMD_OBJ_COUNTER, &(yyvsp[-1].handle), &(yyloc), NULL);
			}
#line 9084 "src/parser_bison.c"
    break;

  case 149: /* delete_cmd: "quota" obj_or_id_spec close_scope_quota  */
#line 1481 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_DELETE, CMD_OBJ_QUOTA, &(yyvsp[-1].handle), &(yyloc), NULL);
			}
#line 9092 "src/parser_bison.c"
    break;

  case 150: /* delete_cmd: "ct" ct_obj_type obj_spec ct_obj_alloc close_scope_ct  */
#line 1485 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc_obj_ct(CMD_DELETE, (yyvsp[-3].val), &(yyvsp[-2].handle), &(yyloc), (yyvsp[-1].obj));
				if ((yyvsp[-3].val) == NFT_OBJECT_CT_TIMEOUT)
					init_list_head(&(yyvsp[-1].obj)->ct_timeout.timeout_list);
			}
#line 9102 "src/parser_bison.c"
    break;

  case 151: /* delete_cmd: "limit" obj_or_id_spec close_scope_limit  */
#line 1491 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_DELETE, CMD_OBJ_LIMIT, &(yyvsp[-1].handle), &(yyloc), NULL);
			}
#line 9110 "src/parser_bison.c"
    break;

  case 152: /* delete_cmd: "secmark" obj_or_id_spec close_scope_secmark  */
#line 1495 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_DELETE, CMD_OBJ_SECMARK, &(yyvsp[-1].handle), &(yyloc), NULL);
			}
#line 9118 "src/parser_bison.c"
    break;

  case 153: /* delete_cmd: "synproxy" obj_or_id_spec close_scope_synproxy  */
#line 1499 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_DELETE, CMD_OBJ_SYNPROXY, &(yyvsp[-1].handle), &(yyloc), NULL);
			}
#line 9126 "src/parser_bison.c"
    break;

  case 154: /* destroy_cmd: "table" table_or_id_spec  */
#line 1505 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_DESTROY, CMD_OBJ_TABLE, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 9134 "src/parser_bison.c"
    break;

  case 155: /* destroy_cmd: "chain" chain_or_id_spec  */
#line 1509 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_DESTROY, CMD_OBJ_CHAIN, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 9142 "src/parser_bison.c"
    break;

  case 156: /* destroy_cmd: "rule" ruleid_spec  */
#line 1513 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_DESTROY, CMD_OBJ_RULE, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 9150 "src/parser_bison.c"
    break;

  case 157: /* destroy_cmd: "set" set_or_id_spec  */
#line 1517 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_DESTROY, CMD_OBJ_SET, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 9158 "src/parser_bison.c"
    break;

  case 158: /* destroy_cmd: "map" set_spec  */
#line 1521 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_DESTROY, CMD_OBJ_SET, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 9166 "src/parser_bison.c"
    break;

  case 159: /* destroy_cmd: "element" set_spec set_block_expr  */
#line 1525 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_DESTROY, CMD_OBJ_ELEMENTS, &(yyvsp[-1].handle), &(yyloc), (yyvsp[0].expr));
			}
#line 9174 "src/parser_bison.c"
    break;

  case 160: /* destroy_cmd: "flowtable" flowtable_spec  */
#line 1529 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_DESTROY, CMD_OBJ_FLOWTABLE, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 9182 "src/parser_bison.c"
    break;

  case 161: /* destroy_cmd: "flowtable" flowtableid_spec  */
#line 1533 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_DESTROY, CMD_OBJ_FLOWTABLE, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 9190 "src/parser_bison.c"
    break;

  case 162: /* destroy_cmd: "flowtable" flowtable_spec flowtable_block_alloc '{' flowtable_block '}'  */
#line 1538 "src/parser_bison.y"
                        {
				(yyvsp[-1].flowtable)->location = (yylsp[-1]);
				handle_merge(&(yyvsp[-3].flowtable)->handle, &(yyvsp[-4].handle));
				(yyval.cmd) = cmd_alloc(CMD_DESTROY, CMD_OBJ_FLOWTABLE, &(yyvsp[-4].handle), &(yyloc), (yyvsp[-1].flowtable));
			}
#line 9200 "src/parser_bison.c"
    break;

  case 163: /* destroy_cmd: "counter" obj_or_id_spec close_scope_counter  */
#line 1544 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_DESTROY, CMD_OBJ_COUNTER, &(yyvsp[-1].handle), &(yyloc), NULL);
			}
#line 9208 "src/parser_bison.c"
    break;

  case 164: /* destroy_cmd: "quota" obj_or_id_spec close_scope_quota  */
#line 1548 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_DESTROY, CMD_OBJ_QUOTA, &(yyvsp[-1].handle), &(yyloc), NULL);
			}
#line 9216 "src/parser_bison.c"
    break;

  case 165: /* destroy_cmd: "ct" ct_obj_type obj_spec ct_obj_alloc close_scope_ct  */
#line 1552 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc_obj_ct(CMD_DESTROY, (yyvsp[-3].val), &(yyvsp[-2].handle), &(yyloc), (yyvsp[-1].obj));
				if ((yyvsp[-3].val) == NFT_OBJECT_CT_TIMEOUT)
					init_list_head(&(yyvsp[-1].obj)->ct_timeout.timeout_list);
			}
#line 9226 "src/parser_bison.c"
    break;

  case 166: /* destroy_cmd: "limit" obj_or_id_spec close_scope_limit  */
#line 1558 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_DESTROY, CMD_OBJ_LIMIT, &(yyvsp[-1].handle), &(yyloc), NULL);
			}
#line 9234 "src/parser_bison.c"
    break;

  case 167: /* destroy_cmd: "secmark" obj_or_id_spec close_scope_secmark  */
#line 1562 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_DESTROY, CMD_OBJ_SECMARK, &(yyvsp[-1].handle), &(yyloc), NULL);
			}
#line 9242 "src/parser_bison.c"
    break;

  case 168: /* destroy_cmd: "synproxy" obj_or_id_spec close_scope_synproxy  */
#line 1566 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_DESTROY, CMD_OBJ_SYNPROXY, &(yyvsp[-1].handle), &(yyloc), NULL);
			}
#line 9250 "src/parser_bison.c"
    break;

  case 169: /* get_cmd: "element" set_spec set_block_expr  */
#line 1573 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_GET, CMD_OBJ_ELEMENTS, &(yyvsp[-1].handle), &(yyloc), (yyvsp[0].expr));
			}
#line 9258 "src/parser_bison.c"
    break;

  case 170: /* list_cmd_spec_table: "table" table_spec  */
#line 1578 "src/parser_bison.y"
                                                        { (yyval.handle) = (yyvsp[0].handle); }
#line 9264 "src/parser_bison.c"
    break;

  case 174: /* list_cmd: "table" table_spec  */
#line 1586 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_LIST, CMD_OBJ_TABLE, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 9272 "src/parser_bison.c"
    break;

  case 175: /* list_cmd: "tables" ruleset_spec  */
#line 1590 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_LIST, CMD_OBJ_TABLE, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 9280 "src/parser_bison.c"
    break;

  case 176: /* list_cmd: "chain" chain_spec  */
#line 1594 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_LIST, CMD_OBJ_CHAIN, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 9288 "src/parser_bison.c"
    break;

  case 177: /* list_cmd: "chains" ruleset_spec  */
#line 1598 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_LIST, CMD_OBJ_CHAINS, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 9296 "src/parser_bison.c"
    break;

  case 178: /* list_cmd: "sets" list_cmd_spec_any  */
#line 1602 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_LIST, CMD_OBJ_SETS, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 9304 "src/parser_bison.c"
    break;

  case 179: /* list_cmd: "set" set_spec  */
#line 1606 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_LIST, CMD_OBJ_SET, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 9312 "src/parser_bison.c"
    break;

  case 180: /* list_cmd: "counters" list_cmd_spec_any  */
#line 1610 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_LIST, CMD_OBJ_COUNTERS, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 9320 "src/parser_bison.c"
    break;

  case 181: /* list_cmd: "counter" obj_spec close_scope_counter  */
#line 1614 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_LIST, CMD_OBJ_COUNTER, &(yyvsp[-1].handle), &(yyloc), NULL);
			}
#line 9328 "src/parser_bison.c"
    break;

  case 182: /* list_cmd: "quotas" list_cmd_spec_any  */
#line 1618 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_LIST, CMD_OBJ_QUOTAS, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 9336 "src/parser_bison.c"
    break;

  case 183: /* list_cmd: "quota" obj_spec close_scope_quota  */
#line 1622 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_LIST, CMD_OBJ_QUOTA, &(yyvsp[-1].handle), &(yyloc), NULL);
			}
#line 9344 "src/parser_bison.c"
    break;

  case 184: /* list_cmd: "limits" list_cmd_spec_any  */
#line 1626 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_LIST, CMD_OBJ_LIMITS, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 9352 "src/parser_bison.c"
    break;

  case 185: /* list_cmd: "limit" obj_spec close_scope_limit  */
#line 1630 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_LIST, CMD_OBJ_LIMIT, &(yyvsp[-1].handle), &(yyloc), NULL);
			}
#line 9360 "src/parser_bison.c"
    break;

  case 186: /* list_cmd: "secmarks" list_cmd_spec_any  */
#line 1634 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_LIST, CMD_OBJ_SECMARKS, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 9368 "src/parser_bison.c"
    break;

  case 187: /* list_cmd: "secmark" obj_spec close_scope_secmark  */
#line 1638 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_LIST, CMD_OBJ_SECMARK, &(yyvsp[-1].handle), &(yyloc), NULL);
			}
#line 9376 "src/parser_bison.c"
    break;

  case 188: /* list_cmd: "synproxys" list_cmd_spec_any  */
#line 1642 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_LIST, CMD_OBJ_SYNPROXYS, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 9384 "src/parser_bison.c"
    break;

  case 189: /* list_cmd: "synproxy" obj_spec close_scope_synproxy  */
#line 1646 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_LIST, CMD_OBJ_SYNPROXY, &(yyvsp[-1].handle), &(yyloc), NULL);
			}
#line 9392 "src/parser_bison.c"
    break;

  case 190: /* list_cmd: "ruleset" ruleset_spec  */
#line 1650 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_LIST, CMD_OBJ_RULESET, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 9400 "src/parser_bison.c"
    break;

  case 191: /* list_cmd: "flow" "tables" ruleset_spec  */
#line 1654 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_LIST, CMD_OBJ_METERS, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 9408 "src/parser_bison.c"
    break;

  case 192: /* list_cmd: "flow" "table" set_spec  */
#line 1658 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_LIST, CMD_OBJ_METER, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 9416 "src/parser_bison.c"
    break;

  case 193: /* list_cmd: "meters" ruleset_spec  */
#line 1662 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_LIST, CMD_OBJ_METERS, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 9424 "src/parser_bison.c"
    break;

  case 194: /* list_cmd: "meter" set_spec  */
#line 1666 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_LIST, CMD_OBJ_METER, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 9432 "src/parser_bison.c"
    break;

  case 195: /* list_cmd: "flowtables" list_cmd_spec_any  */
#line 1670 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_LIST, CMD_OBJ_FLOWTABLES, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 9440 "src/parser_bison.c"
    break;

  case 196: /* list_cmd: "flowtable" flowtable_spec  */
#line 1674 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_LIST, CMD_OBJ_FLOWTABLE, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 9448 "src/parser_bison.c"
    break;

  case 197: /* list_cmd: "maps" list_cmd_spec_any  */
#line 1678 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_LIST, CMD_OBJ_MAPS, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 9456 "src/parser_bison.c"
    break;

  case 198: /* list_cmd: "map" set_spec  */
#line 1682 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_LIST, CMD_OBJ_MAP, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 9464 "src/parser_bison.c"
    break;

  case 199: /* list_cmd: "ct" ct_obj_type obj_spec close_scope_ct  */
#line 1686 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc_obj_ct(CMD_LIST, (yyvsp[-2].val), &(yyvsp[-1].handle), &(yyloc), NULL);
			}
#line 9472 "src/parser_bison.c"
    break;

  case 200: /* list_cmd: "ct" ct_cmd_type "table" table_spec close_scope_ct  */
#line 1690 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_LIST, (yyvsp[-3].val), &(yyvsp[-1].handle), &(yyloc), NULL);
			}
#line 9480 "src/parser_bison.c"
    break;

  case 201: /* list_cmd: "hooks" basehook_spec  */
#line 1694 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_LIST, CMD_OBJ_HOOKS, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 9488 "src/parser_bison.c"
    break;

  case 202: /* basehook_device_name: "device" "string"  */
#line 1700 "src/parser_bison.y"
                        {
				(yyval.string) = (yyvsp[0].string);
			}
#line 9496 "src/parser_bison.c"
    break;

  case 203: /* basehook_spec: ruleset_spec  */
#line 1706 "src/parser_bison.y"
                        {
				(yyval.handle) = (yyvsp[0].handle);
			}
#line 9504 "src/parser_bison.c"
    break;

  case 204: /* basehook_spec: ruleset_spec basehook_device_name  */
#line 1710 "src/parser_bison.y"
                        {
				if ((yyvsp[0].string)) {
					(yyvsp[-1].handle).obj.name = (yyvsp[0].string);
					(yyvsp[-1].handle).obj.location = (yylsp[0]);
				}
				(yyval.handle) = (yyvsp[-1].handle);
			}
#line 9516 "src/parser_bison.c"
    break;

  case 205: /* reset_cmd: "counters" list_cmd_spec_any  */
#line 1720 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_RESET, CMD_OBJ_COUNTERS, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 9524 "src/parser_bison.c"
    break;

  case 206: /* reset_cmd: "counter" obj_spec close_scope_counter  */
#line 1724 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_RESET, CMD_OBJ_COUNTER, &(yyvsp[-1].handle),&(yyloc), NULL);
			}
#line 9532 "src/parser_bison.c"
    break;

  case 207: /* reset_cmd: "quotas" list_cmd_spec_any  */
#line 1728 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_RESET, CMD_OBJ_QUOTAS, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 9540 "src/parser_bison.c"
    break;

  case 208: /* reset_cmd: "quota" obj_spec close_scope_quota  */
#line 1732 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_RESET, CMD_OBJ_QUOTA, &(yyvsp[-1].handle), &(yyloc), NULL);
			}
#line 9548 "src/parser_bison.c"
    break;

  case 209: /* reset_cmd: "rules" ruleset_spec  */
#line 1736 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_RESET, CMD_OBJ_RULES, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 9556 "src/parser_bison.c"
    break;

  case 210: /* reset_cmd: "rules" list_cmd_spec_table  */
#line 1740 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_RESET, CMD_OBJ_TABLE, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 9564 "src/parser_bison.c"
    break;

  case 211: /* reset_cmd: "rules" chain_spec  */
#line 1744 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_RESET, CMD_OBJ_CHAIN, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 9572 "src/parser_bison.c"
    break;

  case 212: /* reset_cmd: "rules" "chain" chain_spec  */
#line 1748 "src/parser_bison.y"
                        {
				/* alias of previous rule. */
				(yyval.cmd) = cmd_alloc(CMD_RESET, CMD_OBJ_CHAIN, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 9581 "src/parser_bison.c"
    break;

  case 213: /* reset_cmd: "rule" ruleid_spec  */
#line 1753 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_RESET, CMD_OBJ_RULE, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 9589 "src/parser_bison.c"
    break;

  case 214: /* reset_cmd: "element" set_spec set_block_expr  */
#line 1757 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_RESET, CMD_OBJ_ELEMENTS, &(yyvsp[-1].handle), &(yyloc), (yyvsp[0].expr));
			}
#line 9597 "src/parser_bison.c"
    break;

  case 215: /* reset_cmd: "set" set_or_id_spec  */
#line 1761 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_RESET, CMD_OBJ_SET, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 9605 "src/parser_bison.c"
    break;

  case 216: /* reset_cmd: "map" set_or_id_spec  */
#line 1765 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_RESET, CMD_OBJ_MAP, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 9613 "src/parser_bison.c"
    break;

  case 217: /* flush_cmd: "table" table_spec  */
#line 1771 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_FLUSH, CMD_OBJ_TABLE, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 9621 "src/parser_bison.c"
    break;

  case 218: /* flush_cmd: "chain" chain_spec  */
#line 1775 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_FLUSH, CMD_OBJ_CHAIN, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 9629 "src/parser_bison.c"
    break;

  case 219: /* flush_cmd: "set" set_spec  */
#line 1779 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_FLUSH, CMD_OBJ_SET, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 9637 "src/parser_bison.c"
    break;

  case 220: /* flush_cmd: "map" set_spec  */
#line 1783 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_FLUSH, CMD_OBJ_MAP, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 9645 "src/parser_bison.c"
    break;

  case 221: /* flush_cmd: "flow" "table" set_spec  */
#line 1787 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_FLUSH, CMD_OBJ_METER, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 9653 "src/parser_bison.c"
    break;

  case 222: /* flush_cmd: "meter" set_spec  */
#line 1791 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_FLUSH, CMD_OBJ_METER, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 9661 "src/parser_bison.c"
    break;

  case 223: /* flush_cmd: "ruleset" ruleset_spec  */
#line 1795 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_FLUSH, CMD_OBJ_RULESET, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 9669 "src/parser_bison.c"
    break;

  case 224: /* rename_cmd: "chain" chain_spec identifier  */
#line 1801 "src/parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_RENAME, CMD_OBJ_CHAIN, &(yyvsp[-1].handle), &(yyloc), NULL);
				(yyval.cmd)->arg = (yyvsp[0].string);
			}
#line 9678 "src/parser_bison.c"
    break;

  case 225: /* import_cmd: "ruleset" markup_format  */
#line 1808 "src/parser_bison.y"
                        {
				struct handle h = { .family = NFPROTO_UNSPEC };
				struct markup *markup = markup_alloc((yyvsp[0].val));
				(yyval.cmd) = cmd_alloc(CMD_IMPORT, CMD_OBJ_MARKUP, &h, &(yyloc), markup);
			}
#line 9688 "src/parser_bison.c"
    break;

  case 226: /* import_cmd: markup_format  */
#line 1814 "src/parser_bison.y"
                        {
				struct handle h = { .family = NFPROTO_UNSPEC };
				struct markup *markup = markup_alloc((yyvsp[0].val));
				(yyval.cmd) = cmd_alloc(CMD_IMPORT, CMD_OBJ_MARKUP, &h, &(yyloc), markup);
			}
#line 9698 "src/parser_bison.c"
    break;

  case 227: /* export_cmd: "ruleset" markup_format  */
#line 1822 "src/parser_bison.y"
                        {
				struct handle h = { .family = NFPROTO_UNSPEC };
				struct markup *markup = markup_alloc((yyvsp[0].val));
				(yyval.cmd) = cmd_alloc(CMD_EXPORT, CMD_OBJ_MARKUP, &h, &(yyloc), markup);
			}
#line 9708 "src/parser_bison.c"
    break;

  case 228: /* export_cmd: markup_format  */
#line 1828 "src/parser_bison.y"
                        {
				struct handle h = { .family = NFPROTO_UNSPEC };
				struct markup *markup = markup_alloc((yyvsp[0].val));
				(yyval.cmd) = cmd_alloc(CMD_EXPORT, CMD_OBJ_MARKUP, &h, &(yyloc), markup);
			}
#line 9718 "src/parser_bison.c"
    break;

  case 229: /* monitor_cmd: monitor_event monitor_object monitor_format  */
#line 1836 "src/parser_bison.y"
                        {
				struct handle h = { .family = NFPROTO_UNSPEC };
				struct monitor *m = monitor_alloc((yyvsp[0].val), (yyvsp[-1].val), (yyvsp[-2].string));
				m->location = (yylsp[-2]);
				(yyval.cmd) = cmd_alloc(CMD_MONITOR, CMD_OBJ_MONITOR, &h, &(yyloc), m);
			}
#line 9729 "src/parser_bison.c"
    break;

  case 230: /* monitor_event: %empty  */
#line 1844 "src/parser_bison.y"
                                                { (yyval.string) = NULL; }
#line 9735 "src/parser_bison.c"
    break;

  case 231: /* monitor_event: "string"  */
#line 1845 "src/parser_bison.y"
                                                { (yyval.string) = (yyvsp[0].string); }
#line 9741 "src/parser_bison.c"
    break;

  case 232: /* monitor_object: %empty  */
#line 1848 "src/parser_bison.y"
                                                { (yyval.val) = CMD_MONITOR_OBJ_ANY; }
#line 9747 "src/parser_bison.c"
    break;

  case 233: /* monitor_object: "tables"  */
#line 1849 "src/parser_bison.y"
                                                { (yyval.val) = CMD_MONITOR_OBJ_TABLES; }
#line 9753 "src/parser_bison.c"
    break;

  case 234: /* monitor_object: "chains"  */
#line 1850 "src/parser_bison.y"
                                                { (yyval.val) = CMD_MONITOR_OBJ_CHAINS; }
#line 9759 "src/parser_bison.c"
    break;

  case 235: /* monitor_object: "sets"  */
#line 1851 "src/parser_bison.y"
                                                { (yyval.val) = CMD_MONITOR_OBJ_SETS; }
#line 9765 "src/parser_bison.c"
    break;

  case 236: /* monitor_object: "rules"  */
#line 1852 "src/parser_bison.y"
                                                { (yyval.val) = CMD_MONITOR_OBJ_RULES; }
#line 9771 "src/parser_bison.c"
    break;

  case 237: /* monitor_object: "elements"  */
#line 1853 "src/parser_bison.y"
                                                { (yyval.val) = CMD_MONITOR_OBJ_ELEMS; }
#line 9777 "src/parser_bison.c"
    break;

  case 238: /* monitor_object: "ruleset"  */
#line 1854 "src/parser_bison.y"
                                                { (yyval.val) = CMD_MONITOR_OBJ_RULESET; }
#line 9783 "src/parser_bison.c"
    break;

  case 239: /* monitor_object: "trace"  */
#line 1855 "src/parser_bison.y"
                                                { (yyval.val) = CMD_MONITOR_OBJ_TRACE; }
#line 9789 "src/parser_bison.c"
    break;

  case 240: /* monitor_format: %empty  */
#line 1858 "src/parser_bison.y"
                                                { (yyval.val) = NFTNL_OUTPUT_DEFAULT; }
#line 9795 "src/parser_bison.c"
    break;

  case 242: /* markup_format: "xml"  */
#line 1862 "src/parser_bison.y"
                                                { (yyval.val) = __NFT_OUTPUT_NOTSUPP; }
#line 9801 "src/parser_bison.c"
    break;

  case 243: /* markup_format: "json"  */
#line 1863 "src/parser_bison.y"
                                                { (yyval.val) = NFTNL_OUTPUT_JSON; }
#line 9807 "src/parser_bison.c"
    break;

  case 244: /* markup_format: "vm" "json"  */
#line 1864 "src/parser_bison.y"
                                                { (yyval.val) = NFTNL_OUTPUT_JSON; }
#line 9813 "src/parser_bison.c"
    break;

  case 245: /* describe_cmd: primary_expr  */
#line 1868 "src/parser_bison.y"
                        {
				struct handle h = { .family = NFPROTO_UNSPEC };
				(yyval.cmd) = cmd_alloc(CMD_DESCRIBE, CMD_OBJ_EXPR, &h, &(yyloc), NULL);
				(yyval.cmd)->expr = (yyvsp[0].expr);
			}
#line 9823 "src/parser_bison.c"
    break;

  case 246: /* table_block_alloc: %empty  */
#line 1876 "src/parser_bison.y"
                        {
				(yyval.table) = table_alloc();
				if (open_scope(state, &(yyval.table)->scope) < 0) {
					erec_queue(error(&(yyloc), "too many levels of nesting"),
						   state->msgs);
					state->nerrs++;
				}
			}
#line 9836 "src/parser_bison.c"
    break;

  case 247: /* table_options: "flags" table_flags  */
#line 1887 "src/parser_bison.y"
                        {
				(yyvsp[-2].table)->flags |= (yyvsp[0].val);
			}
#line 9844 "src/parser_bison.c"
    break;

  case 248: /* table_options: comment_spec  */
#line 1891 "src/parser_bison.y"
                        {
				if (already_set((yyvsp[-1].table)->comment, &(yyloc), state)) {
					free_const((yyvsp[0].string));
					YYERROR;
				}
				(yyvsp[-1].table)->comment = (yyvsp[0].string);
			}
#line 9856 "src/parser_bison.c"
    break;

  case 250: /* table_flags: table_flags "comma" table_flag  */
#line 1902 "src/parser_bison.y"
                        {
				(yyval.val) = (yyvsp[-2].val) | (yyvsp[0].val);
			}
#line 9864 "src/parser_bison.c"
    break;

  case 251: /* table_flag: "string"  */
#line 1907 "src/parser_bison.y"
                        {
				(yyval.val) = parse_table_flag((yyvsp[0].string));
				if ((yyval.val) == 0) {
					erec_queue(error(&(yylsp[0]), "unknown table option %s", (yyvsp[0].string)),
						   state->msgs);
					free_const((yyvsp[0].string));
					YYERROR;
				}

				free_const((yyvsp[0].string));
			}
#line 9880 "src/parser_bison.c"
    break;

  case 252: /* table_block: %empty  */
#line 1920 "src/parser_bison.y"
                                                { (yyval.table) = (yyvsp[(-1) - (0)].table); }
#line 9886 "src/parser_bison.c"
    break;

  case 256: /* table_block: table_block "chain" chain_identifier chain_block_alloc '{' chain_block '}' stmt_separator  */
#line 1927 "src/parser_bison.y"
                        {
				(yyvsp[-4].chain)->location = (yylsp[-5]);
				handle_merge(&(yyvsp[-4].chain)->handle, &(yyvsp[-5].handle));
				handle_free(&(yyvsp[-5].handle));
				close_scope(state);
				list_add_tail(&(yyvsp[-4].chain)->list, &(yyvsp[-7].table)->chains);
				(yyval.table) = (yyvsp[-7].table);
			}
#line 9899 "src/parser_bison.c"
    break;

  case 257: /* table_block: table_block "set" set_identifier set_block_alloc '{' set_block '}' stmt_separator  */
#line 1938 "src/parser_bison.y"
                        {
				(yyvsp[-4].set)->location = (yylsp[-5]);
				handle_merge(&(yyvsp[-4].set)->handle, &(yyvsp[-5].handle));
				handle_free(&(yyvsp[-5].handle));
				list_add_tail(&(yyvsp[-4].set)->list, &(yyvsp[-7].table)->sets);
				(yyval.table) = (yyvsp[-7].table);
			}
#line 9911 "src/parser_bison.c"
    break;

  case 258: /* table_block: table_block "map" set_identifier map_block_alloc '{' map_block '}' stmt_separator  */
#line 1948 "src/parser_bison.y"
                        {
				(yyvsp[-4].set)->location = (yylsp[-5]);
				handle_merge(&(yyvsp[-4].set)->handle, &(yyvsp[-5].handle));
				handle_free(&(yyvsp[-5].handle));
				list_add_tail(&(yyvsp[-4].set)->list, &(yyvsp[-7].table)->sets);
				(yyval.table) = (yyvsp[-7].table);
			}
#line 9923 "src/parser_bison.c"
    break;

  case 259: /* table_block: table_block "flowtable" flowtable_identifier flowtable_block_alloc '{' flowtable_block '}' stmt_separator  */
#line 1959 "src/parser_bison.y"
                        {
				(yyvsp[-4].flowtable)->location = (yylsp[-5]);
				handle_merge(&(yyvsp[-4].flowtable)->handle, &(yyvsp[-5].handle));
				handle_free(&(yyvsp[-5].handle));
				list_add_tail(&(yyvsp[-4].flowtable)->list, &(yyvsp[-7].table)->flowtables);
				(yyval.table) = (yyvsp[-7].table);
			}
#line 9935 "src/parser_bison.c"
    break;

  case 260: /* table_block: table_block "counter" obj_identifier obj_block_alloc '{' counter_block '}' stmt_separator close_scope_counter  */
#line 1969 "src/parser_bison.y"
                        {
				(yyvsp[-5].obj)->location = (yylsp[-6]);
				(yyvsp[-5].obj)->type = NFT_OBJECT_COUNTER;
				handle_merge(&(yyvsp[-5].obj)->handle, &(yyvsp[-6].handle));
				handle_free(&(yyvsp[-6].handle));
				list_add_tail(&(yyvsp[-5].obj)->list, &(yyvsp[-8].table)->objs);
				(yyval.table) = (yyvsp[-8].table);
			}
#line 9948 "src/parser_bison.c"
    break;

  case 261: /* table_block: table_block "quota" obj_identifier obj_block_alloc '{' quota_block '}' stmt_separator close_scope_quota  */
#line 1980 "src/parser_bison.y"
                        {
				(yyvsp[-5].obj)->location = (yylsp[-6]);
				(yyvsp[-5].obj)->type = NFT_OBJECT_QUOTA;
				handle_merge(&(yyvsp[-5].obj)->handle, &(yyvsp[-6].handle));
				handle_free(&(yyvsp[-6].handle));
				list_add_tail(&(yyvsp[-5].obj)->list, &(yyvsp[-8].table)->objs);
				(yyval.table) = (yyvsp[-8].table);
			}
#line 9961 "src/parser_bison.c"
    break;

  case 262: /* table_block: table_block "ct" "helper" obj_identifier obj_block_alloc '{' ct_helper_block '}' stmt_separator close_scope_ct  */
#line 1989 "src/parser_bison.y"
                        {
				(yyvsp[-5].obj)->location = (yylsp[-6]);
				(yyvsp[-5].obj)->type = NFT_OBJECT_CT_HELPER;
				handle_merge(&(yyvsp[-5].obj)->handle, &(yyvsp[-6].handle));
				handle_free(&(yyvsp[-6].handle));
				list_add_tail(&(yyvsp[-5].obj)->list, &(yyvsp[-9].table)->objs);
				(yyval.table) = (yyvsp[-9].table);
			}
#line 9974 "src/parser_bison.c"
    break;

  case 263: /* table_block: table_block "ct" "timeout" obj_identifier obj_block_alloc '{' ct_timeout_block '}' stmt_separator close_scope_ct  */
#line 1998 "src/parser_bison.y"
                        {
				(yyvsp[-5].obj)->location = (yylsp[-6]);
				(yyvsp[-5].obj)->type = NFT_OBJECT_CT_TIMEOUT;
				handle_merge(&(yyvsp[-5].obj)->handle, &(yyvsp[-6].handle));
				handle_free(&(yyvsp[-6].handle));
				list_add_tail(&(yyvsp[-5].obj)->list, &(yyvsp[-9].table)->objs);
				(yyval.table) = (yyvsp[-9].table);
			}
#line 9987 "src/parser_bison.c"
    break;

  case 264: /* table_block: table_block "ct" "expectation" obj_identifier obj_block_alloc '{' ct_expect_block '}' stmt_separator close_scope_ct  */
#line 2007 "src/parser_bison.y"
                        {
				(yyvsp[-5].obj)->location = (yylsp[-6]);
				(yyvsp[-5].obj)->type = NFT_OBJECT_CT_EXPECT;
				handle_merge(&(yyvsp[-5].obj)->handle, &(yyvsp[-6].handle));
				handle_free(&(yyvsp[-6].handle));
				list_add_tail(&(yyvsp[-5].obj)->list, &(yyvsp[-9].table)->objs);
				(yyval.table) = (yyvsp[-9].table);
			}
#line 10000 "src/parser_bison.c"
    break;

  case 265: /* table_block: table_block "limit" obj_identifier obj_block_alloc '{' limit_block '}' stmt_separator close_scope_limit  */
#line 2018 "src/parser_bison.y"
                        {
				(yyvsp[-5].obj)->location = (yylsp[-6]);
				(yyvsp[-5].obj)->type = NFT_OBJECT_LIMIT;
				handle_merge(&(yyvsp[-5].obj)->handle, &(yyvsp[-6].handle));
				handle_free(&(yyvsp[-6].handle));
				list_add_tail(&(yyvsp[-5].obj)->list, &(yyvsp[-8].table)->objs);
				(yyval.table) = (yyvsp[-8].table);
			}
#line 10013 "src/parser_bison.c"
    break;

  case 266: /* table_block: table_block "secmark" obj_identifier obj_block_alloc '{' secmark_block '}' stmt_separator close_scope_secmark  */
#line 2029 "src/parser_bison.y"
                        {
				(yyvsp[-5].obj)->location = (yylsp[-6]);
				(yyvsp[-5].obj)->type = NFT_OBJECT_SECMARK;
				handle_merge(&(yyvsp[-5].obj)->handle, &(yyvsp[-6].handle));
				handle_free(&(yyvsp[-6].handle));
				list_add_tail(&(yyvsp[-5].obj)->list, &(yyvsp[-8].table)->objs);
				(yyval.table) = (yyvsp[-8].table);
			}
#line 10026 "src/parser_bison.c"
    break;

  case 267: /* table_block: table_block "synproxy" obj_identifier obj_block_alloc '{' synproxy_block '}' stmt_separator close_scope_synproxy  */
#line 2040 "src/parser_bison.y"
                        {
				(yyvsp[-5].obj)->location = (yylsp[-6]);
				(yyvsp[-5].obj)->type = NFT_OBJECT_SYNPROXY;
				handle_merge(&(yyvsp[-5].obj)->handle, &(yyvsp[-6].handle));
				handle_free(&(yyvsp[-6].handle));
				list_add_tail(&(yyvsp[-5].obj)->list, &(yyvsp[-8].table)->objs);
				(yyval.table) = (yyvsp[-8].table);
			}
#line 10039 "src/parser_bison.c"
    break;

  case 268: /* chain_block_alloc: %empty  */
#line 2051 "src/parser_bison.y"
                        {
				(yyval.chain) = chain_alloc();
				if (open_scope(state, &(yyval.chain)->scope) < 0) {
					erec_queue(error(&(yyloc), "too many levels of nesting"),
						   state->msgs);
					state->nerrs++;
				}
			}
#line 10052 "src/parser_bison.c"
    break;

  case 269: /* chain_block: %empty  */
#line 2061 "src/parser_bison.y"
                                                { (yyval.chain) = (yyvsp[(-1) - (0)].chain); }
#line 10058 "src/parser_bison.c"
    break;

  case 275: /* chain_block: chain_block rule stmt_separator  */
#line 2068 "src/parser_bison.y"
                        {
				list_add_tail(&(yyvsp[-1].rule)->list, &(yyvsp[-2].chain)->rules);
				(yyval.chain) = (yyvsp[-2].chain);
			}
#line 10067 "src/parser_bison.c"
    break;

  case 276: /* chain_block: chain_block "devices" '=' flowtable_expr stmt_separator  */
#line 2073 "src/parser_bison.y"
                        {
				if ((yyval.chain)->dev_expr) {
					list_splice_init(&(yyvsp[-1].expr)->expressions, &(yyval.chain)->dev_expr->expressions);
					expr_free((yyvsp[-1].expr));
					break;
				}
				(yyval.chain)->dev_expr = (yyvsp[-1].expr);
			}
#line 10080 "src/parser_bison.c"
    break;

  case 277: /* chain_block: chain_block comment_spec stmt_separator  */
#line 2082 "src/parser_bison.y"
                        {
				if (already_set((yyvsp[-2].chain)->comment, &(yylsp[-1]), state)) {
					free_const((yyvsp[-1].string));
					YYERROR;
				}
				(yyvsp[-2].chain)->comment = (yyvsp[-1].string);
			}
#line 10092 "src/parser_bison.c"
    break;

  case 278: /* subchain_block: %empty  */
#line 2091 "src/parser_bison.y"
                                                { (yyval.chain) = (yyvsp[(-1) - (0)].chain); }
#line 10098 "src/parser_bison.c"
    break;

  case 280: /* subchain_block: subchain_block rule stmt_separator  */
#line 2094 "src/parser_bison.y"
                        {
				list_add_tail(&(yyvsp[-1].rule)->list, &(yyvsp[-2].chain)->rules);
				(yyval.chain) = (yyvsp[-2].chain);
			}
#line 10107 "src/parser_bison.c"
    break;

  case 281: /* typeof_verdict_expr: selector_expr  */
#line 2101 "src/parser_bison.y"
                        {
				struct expr *e = (yyvsp[0].expr);

				if (expr_ops(e)->build_udata == NULL) {
					erec_queue(error(&(yylsp[0]), "map data type '%s' lacks typeof serialization", expr_ops(e)->name),
						   state->msgs);
					expr_free(e);
					YYERROR;
				}
				(yyval.expr) = e;
			}
#line 10123 "src/parser_bison.c"
    break;

  case 282: /* typeof_verdict_expr: typeof_expr "." selector_expr  */
#line 2113 "src/parser_bison.y"
                        {
				struct location rhs[] = {
					[1]	= (yylsp[-1]),
					[2]	= (yylsp[0]),
				};

				(yyval.expr) = handle_concat_expr(&(yyloc), (yyval.expr), (yyvsp[-2].expr), (yyvsp[0].expr), rhs);
			}
#line 10136 "src/parser_bison.c"
    break;

  case 283: /* typeof_data_expr: "interval" typeof_expr  */
#line 2124 "src/parser_bison.y"
                        {
				(yyvsp[0].expr)->flags |= EXPR_F_INTERVAL;
				(yyval.expr) = (yyvsp[0].expr);
			}
#line 10145 "src/parser_bison.c"
    break;

  case 284: /* typeof_data_expr: typeof_verdict_expr  */
#line 2129 "src/parser_bison.y"
                        {
				(yyval.expr) = (yyvsp[0].expr);
			}
#line 10153 "src/parser_bison.c"
    break;

  case 285: /* typeof_data_expr: "queue"  */
#line 2133 "src/parser_bison.y"
                        {
				(yyval.expr) = constant_expr_alloc(&(yyloc), &queue_type, BYTEORDER_HOST_ENDIAN, 16, NULL);
			}
#line 10161 "src/parser_bison.c"
    break;

  case 286: /* typeof_data_expr: "string"  */
#line 2137 "src/parser_bison.y"
                        {
				struct expr *verdict;

				if (strcmp("verdict", (yyvsp[0].string)) != 0) {
					erec_queue(error(&(yylsp[0]), "map data type '%s' lacks typeof serialization", (yyvsp[0].string)),
						   state->msgs);
					free_const((yyvsp[0].string));
					YYERROR;
				}
				verdict = verdict_expr_alloc(&(yylsp[0]), NF_ACCEPT, NULL);
				verdict->flags &= ~EXPR_F_CONSTANT;
				(yyval.expr) = verdict;
				free_const((yyvsp[0].string));
			}
#line 10180 "src/parser_bison.c"
    break;

  case 287: /* primary_typeof_expr: selector_expr  */
#line 2154 "src/parser_bison.y"
                        {
				if (expr_ops((yyvsp[0].expr))->build_udata == NULL) {
					erec_queue(error(&(yylsp[0]), "primary expression type '%s' lacks typeof serialization", expr_ops((yyvsp[0].expr))->name),
						   state->msgs);
					expr_free((yyvsp[0].expr));
					YYERROR;
				}

				(yyval.expr) = (yyvsp[0].expr);
			}
#line 10195 "src/parser_bison.c"
    break;

  case 288: /* typeof_expr: primary_typeof_expr  */
#line 2167 "src/parser_bison.y"
                        {
				(yyval.expr) = (yyvsp[0].expr);
			}
#line 10203 "src/parser_bison.c"
    break;

  case 289: /* typeof_expr: typeof_expr "." primary_typeof_expr  */
#line 2171 "src/parser_bison.y"
                        {
				struct location rhs[] = {
					[1]	= (yylsp[-1]),
					[2]	= (yylsp[0]),
				};

				(yyval.expr) = handle_concat_expr(&(yyloc), (yyval.expr), (yyvsp[-2].expr), (yyvsp[0].expr), rhs);
			}
#line 10216 "src/parser_bison.c"
    break;

  case 290: /* set_block_alloc: %empty  */
#line 2183 "src/parser_bison.y"
                        {
				(yyval.set) = set_alloc(&internal_location);
			}
#line 10224 "src/parser_bison.c"
    break;

  case 291: /* typeof_key_expr: "typeof" typeof_expr  */
#line 2188 "src/parser_bison.y"
                                                    { (yyval.expr) = (yyvsp[0].expr); }
#line 10230 "src/parser_bison.c"
    break;

  case 292: /* typeof_key_expr: "type" data_type_expr close_scope_type  */
#line 2189 "src/parser_bison.y"
                                                                        { (yyval.expr) = (yyvsp[-1].expr); }
#line 10236 "src/parser_bison.c"
    break;

  case 293: /* set_block: %empty  */
#line 2192 "src/parser_bison.y"
                                                { (yyval.set) = (yyvsp[(-1) - (0)].set); }
#line 10242 "src/parser_bison.c"
    break;

  case 296: /* set_block: set_block typeof_key_expr stmt_separator  */
#line 2196 "src/parser_bison.y"
                        {
				if (already_set((yyvsp[-2].set)->key, &(yylsp[-1]), state)) {
					expr_free((yyvsp[-1].expr));
					YYERROR;
				}

				(yyvsp[-2].set)->key = (yyvsp[-1].expr);
				(yyval.set) = (yyvsp[-2].set);
			}
#line 10256 "src/parser_bison.c"
    break;

  case 297: /* set_block: set_block "flags" set_flag_list stmt_separator  */
#line 2206 "src/parser_bison.y"
                        {
				(yyvsp[-3].set)->flags = (yyvsp[-1].val);
				(yyval.set) = (yyvsp[-3].set);
			}
#line 10265 "src/parser_bison.c"
    break;

  case 298: /* set_block: set_block "timeout" time_spec stmt_separator  */
#line 2211 "src/parser_bison.y"
                        {
				(yyvsp[-3].set)->timeout = (yyvsp[-1].val);
				(yyval.set) = (yyvsp[-3].set);
			}
#line 10274 "src/parser_bison.c"
    break;

  case 299: /* set_block: set_block "gc-interval" time_spec stmt_separator  */
#line 2216 "src/parser_bison.y"
                        {
				(yyvsp[-3].set)->gc_int = (yyvsp[-1].val);
				(yyval.set) = (yyvsp[-3].set);
			}
#line 10283 "src/parser_bison.c"
    break;

  case 300: /* set_block: set_block stateful_stmt_list stmt_separator  */
#line 2221 "src/parser_bison.y"
                        {
				list_splice_tail((yyvsp[-1].list), &(yyvsp[-2].set)->stmt_list);
				(yyval.set) = (yyvsp[-2].set);
				free((yyvsp[-1].list));
			}
#line 10293 "src/parser_bison.c"
    break;

  case 301: /* set_block: set_block "elements" '=' set_block_expr  */
#line 2227 "src/parser_bison.y"
                        {
				if (already_set((yyvsp[-3].set)->init, &(yylsp[-2]), state)) {
					expr_free((yyvsp[0].expr));
					YYERROR;
				}
				(yyvsp[-3].set)->init = (yyvsp[0].expr);
				(yyval.set) = (yyvsp[-3].set);
			}
#line 10306 "src/parser_bison.c"
    break;

  case 302: /* set_block: set_block "auto-merge"  */
#line 2236 "src/parser_bison.y"
                        {
				(yyvsp[-1].set)->automerge = true;
				(yyval.set) = (yyvsp[-1].set);
			}
#line 10315 "src/parser_bison.c"
    break;

  case 304: /* set_block: set_block comment_spec stmt_separator  */
#line 2242 "src/parser_bison.y"
                        {
				if (already_set((yyvsp[-2].set)->comment, &(yylsp[-1]), state)) {
					free_const((yyvsp[-1].string));
					YYERROR;
				}
				(yyvsp[-2].set)->comment = (yyvsp[-1].string);
				(yyval.set) = (yyvsp[-2].set);
			}
#line 10328 "src/parser_bison.c"
    break;

  case 307: /* set_flag_list: set_flag_list "comma" set_flag  */
#line 2257 "src/parser_bison.y"
                        {
				(yyval.val) = (yyvsp[-2].val) | (yyvsp[0].val);
			}
#line 10336 "src/parser_bison.c"
    break;

  case 309: /* set_flag: "constant"  */
#line 2263 "src/parser_bison.y"
                                                { (yyval.val) = NFT_SET_CONSTANT; }
#line 10342 "src/parser_bison.c"
    break;

  case 310: /* set_flag: "interval"  */
#line 2264 "src/parser_bison.y"
                                                { (yyval.val) = NFT_SET_INTERVAL; }
#line 10348 "src/parser_bison.c"
    break;

  case 311: /* set_flag: "timeout"  */
#line 2265 "src/parser_bison.y"
                                                { (yyval.val) = NFT_SET_TIMEOUT; }
#line 10354 "src/parser_bison.c"
    break;

  case 312: /* set_flag: "dynamic"  */
#line 2266 "src/parser_bison.y"
                                                { (yyval.val) = NFT_SET_EVAL; }
#line 10360 "src/parser_bison.c"
    break;

  case 313: /* map_block_alloc: %empty  */
#line 2270 "src/parser_bison.y"
                        {
				(yyval.set) = set_alloc(&internal_location);
			}
#line 10368 "src/parser_bison.c"
    break;

  case 314: /* ct_obj_type_map: "timeout"  */
#line 2275 "src/parser_bison.y"
                                                { (yyval.val) = NFT_OBJECT_CT_TIMEOUT; }
#line 10374 "src/parser_bison.c"
    break;

  case 315: /* ct_obj_type_map: "expectation"  */
#line 2276 "src/parser_bison.y"
                                                { (yyval.val) = NFT_OBJECT_CT_EXPECT; }
#line 10380 "src/parser_bison.c"
    break;

  case 316: /* map_block_obj_type: "counter" close_scope_counter  */
#line 2279 "src/parser_bison.y"
                                                            { (yyval.val) = NFT_OBJECT_COUNTER; }
#line 10386 "src/parser_bison.c"
    break;

  case 317: /* map_block_obj_type: "quota" close_scope_quota  */
#line 2280 "src/parser_bison.y"
                                                          { (yyval.val) = NFT_OBJECT_QUOTA; }
#line 10392 "src/parser_bison.c"
    break;

  case 318: /* map_block_obj_type: "limit" close_scope_limit  */
#line 2281 "src/parser_bison.y"
                                                          { (yyval.val) = NFT_OBJECT_LIMIT; }
#line 10398 "src/parser_bison.c"
    break;

  case 319: /* map_block_obj_type: "secmark" close_scope_secmark  */
#line 2282 "src/parser_bison.y"
                                                            { (yyval.val) = NFT_OBJECT_SECMARK; }
#line 10404 "src/parser_bison.c"
    break;

  case 320: /* map_block_obj_type: "synproxy" close_scope_synproxy  */
#line 2283 "src/parser_bison.y"
                                                              { (yyval.val) = NFT_OBJECT_SYNPROXY; }
#line 10410 "src/parser_bison.c"
    break;

  case 322: /* map_block_obj_typeof: "ct" ct_obj_type_map close_scope_ct  */
#line 2287 "src/parser_bison.y"
                                                                        { (yyval.val) = (yyvsp[-1].val); }
#line 10416 "src/parser_bison.c"
    break;

  case 323: /* map_block_data_interval: "interval"  */
#line 2290 "src/parser_bison.y"
                                         { (yyval.val) = EXPR_F_INTERVAL; }
#line 10422 "src/parser_bison.c"
    break;

  case 324: /* map_block_data_interval: %empty  */
#line 2291 "src/parser_bison.y"
                                { (yyval.val) = 0; }
#line 10428 "src/parser_bison.c"
    break;

  case 325: /* map_block: %empty  */
#line 2294 "src/parser_bison.y"
                                                { (yyval.set) = (yyvsp[(-1) - (0)].set); }
#line 10434 "src/parser_bison.c"
    break;

  case 328: /* map_block: map_block "timeout" time_spec stmt_separator  */
#line 2298 "src/parser_bison.y"
                        {
				(yyvsp[-3].set)->timeout = (yyvsp[-1].val);
				(yyval.set) = (yyvsp[-3].set);
			}
#line 10443 "src/parser_bison.c"
    break;

  case 329: /* map_block: map_block "gc-interval" time_spec stmt_separator  */
#line 2303 "src/parser_bison.y"
                        {
				(yyvsp[-3].set)->gc_int = (yyvsp[-1].val);
				(yyval.set) = (yyvsp[-3].set);
			}
#line 10452 "src/parser_bison.c"
    break;

  case 330: /* map_block: map_block "type" data_type_expr "colon" map_block_data_interval data_type_expr stmt_separator close_scope_type  */
#line 2310 "src/parser_bison.y"
                        {
				if (already_set((yyvsp[-7].set)->key, &(yylsp[-6]), state)) {
					expr_free((yyvsp[-5].expr));
					expr_free((yyvsp[-2].expr));
					YYERROR;
				}

				(yyvsp[-7].set)->key = (yyvsp[-5].expr);
				(yyvsp[-7].set)->data = (yyvsp[-2].expr);
				(yyvsp[-7].set)->data->flags |= (yyvsp[-3].val);

				(yyvsp[-7].set)->flags |= NFT_SET_MAP;
				(yyval.set) = (yyvsp[-7].set);
			}
#line 10471 "src/parser_bison.c"
    break;

  case 331: /* map_block: map_block "typeof" typeof_expr "colon" typeof_data_expr stmt_separator  */
#line 2327 "src/parser_bison.y"
                        {
				if (already_set((yyvsp[-5].set)->key, &(yylsp[-4]), state)) {
					expr_free((yyvsp[-3].expr));
					expr_free((yyvsp[-1].expr));
					YYERROR;
				}

				(yyvsp[-5].set)->key = (yyvsp[-3].expr);

				if ((yyvsp[-1].expr)->etype == EXPR_CT && (yyvsp[-1].expr)->ct.key == NFT_CT_HELPER) {
					(yyvsp[-5].set)->objtype = NFT_OBJECT_CT_HELPER;
					(yyvsp[-5].set)->flags  |= NFT_SET_OBJECT;
					expr_free((yyvsp[-1].expr));
				} else {
					(yyvsp[-5].set)->data = (yyvsp[-1].expr);
					(yyvsp[-5].set)->flags |= NFT_SET_MAP;
				}

				(yyval.set) = (yyvsp[-5].set);
			}
#line 10496 "src/parser_bison.c"
    break;

  case 332: /* map_block: map_block "type" data_type_expr "colon" map_block_obj_type stmt_separator close_scope_type  */
#line 2350 "src/parser_bison.y"
                        {
				if (already_set((yyvsp[-6].set)->key, &(yylsp[-5]), state)) {
					expr_free((yyvsp[-4].expr));
					YYERROR;
				}

				(yyvsp[-6].set)->key = (yyvsp[-4].expr);
				(yyvsp[-6].set)->objtype = (yyvsp[-2].val);
				(yyvsp[-6].set)->flags  |= NFT_SET_OBJECT;
				(yyval.set) = (yyvsp[-6].set);
			}
#line 10512 "src/parser_bison.c"
    break;

  case 333: /* map_block: map_block "typeof" typeof_expr "colon" map_block_obj_typeof stmt_separator  */
#line 2364 "src/parser_bison.y"
                        {
				(yyvsp[-5].set)->key = (yyvsp[-3].expr);
				(yyvsp[-5].set)->objtype = (yyvsp[-1].val);
				(yyvsp[-5].set)->flags  |= NFT_SET_OBJECT;
				(yyval.set) = (yyvsp[-5].set);
			}
#line 10523 "src/parser_bison.c"
    break;

  case 334: /* map_block: map_block "flags" set_flag_list stmt_separator  */
#line 2371 "src/parser_bison.y"
                        {
				(yyvsp[-3].set)->flags |= (yyvsp[-1].val);
				(yyval.set) = (yyvsp[-3].set);
			}
#line 10532 "src/parser_bison.c"
    break;

  case 335: /* map_block: map_block stateful_stmt_list stmt_separator  */
#line 2376 "src/parser_bison.y"
                        {
				list_splice_tail((yyvsp[-1].list), &(yyvsp[-2].set)->stmt_list);
				(yyval.set) = (yyvsp[-2].set);
				free((yyvsp[-1].list));
			}
#line 10542 "src/parser_bison.c"
    break;

  case 336: /* map_block: map_block "elements" '=' set_block_expr  */
#line 2382 "src/parser_bison.y"
                        {
				(yyvsp[-3].set)->init = (yyvsp[0].expr);
				(yyval.set) = (yyvsp[-3].set);
			}
#line 10551 "src/parser_bison.c"
    break;

  case 337: /* map_block: map_block comment_spec stmt_separator  */
#line 2387 "src/parser_bison.y"
                        {
				if (already_set((yyvsp[-2].set)->comment, &(yylsp[-1]), state)) {
					free_const((yyvsp[-1].string));
					YYERROR;
				}
				(yyvsp[-2].set)->comment = (yyvsp[-1].string);
				(yyval.set) = (yyvsp[-2].set);
			}
#line 10564 "src/parser_bison.c"
    break;

  case 339: /* set_mechanism: "policy" set_policy_spec close_scope_policy  */
#line 2399 "src/parser_bison.y"
                        {
				(yyvsp[-3].set)->policy = (yyvsp[-1].val);
			}
#line 10572 "src/parser_bison.c"
    break;

  case 340: /* set_mechanism: "size" "number"  */
#line 2403 "src/parser_bison.y"
                        {
				(yyvsp[-2].set)->desc.size = (yyvsp[0].val);
			}
#line 10580 "src/parser_bison.c"
    break;

  case 341: /* set_policy_spec: "performance"  */
#line 2408 "src/parser_bison.y"
                                                { (yyval.val) = NFT_SET_POL_PERFORMANCE; }
#line 10586 "src/parser_bison.c"
    break;

  case 342: /* set_policy_spec: "memory"  */
#line 2409 "src/parser_bison.y"
                                                { (yyval.val) = NFT_SET_POL_MEMORY; }
#line 10592 "src/parser_bison.c"
    break;

  case 343: /* flowtable_block_alloc: %empty  */
#line 2413 "src/parser_bison.y"
                        {
				(yyval.flowtable) = flowtable_alloc(&internal_location);
			}
#line 10600 "src/parser_bison.c"
    break;

  case 344: /* flowtable_block: %empty  */
#line 2418 "src/parser_bison.y"
                                                { (yyval.flowtable) = (yyvsp[(-1) - (0)].flowtable); }
#line 10606 "src/parser_bison.c"
    break;

  case 347: /* flowtable_block: flowtable_block "hook" "string" prio_spec stmt_separator  */
#line 2422 "src/parser_bison.y"
                        {
				(yyval.flowtable)->hook.loc = (yylsp[-2]);
				(yyval.flowtable)->hook.name = chain_hookname_lookup((yyvsp[-2].string));
				if ((yyval.flowtable)->hook.name == NULL) {
					erec_queue(error(&(yylsp[-2]), "unknown chain hook"),
						   state->msgs);
					free_const((yyvsp[-2].string));
					YYERROR;
				}
				free_const((yyvsp[-2].string));

				(yyval.flowtable)->priority = (yyvsp[-1].prio_spec);
			}
#line 10624 "src/parser_bison.c"
    break;

  case 348: /* flowtable_block: flowtable_block "devices" '=' flowtable_expr stmt_separator  */
#line 2436 "src/parser_bison.y"
                        {
				(yyval.flowtable)->dev_expr = (yyvsp[-1].expr);
			}
#line 10632 "src/parser_bison.c"
    break;

  case 349: /* flowtable_block: flowtable_block "counter" close_scope_counter  */
#line 2440 "src/parser_bison.y"
                        {
				(yyval.flowtable)->flags |= NFT_FLOWTABLE_COUNTER;
			}
#line 10640 "src/parser_bison.c"
    break;

  case 350: /* flowtable_block: flowtable_block "flags" "offload" stmt_separator  */
#line 2444 "src/parser_bison.y"
                        {
				(yyval.flowtable)->flags |= FLOWTABLE_F_HW_OFFLOAD;
			}
#line 10648 "src/parser_bison.c"
    break;

  case 351: /* flowtable_expr: '{' flowtable_list_expr '}'  */
#line 2450 "src/parser_bison.y"
                        {
				(yyvsp[-1].expr)->location = (yyloc);
				(yyval.expr) = (yyvsp[-1].expr);
			}
#line 10657 "src/parser_bison.c"
    break;

  case 352: /* flowtable_expr: variable_expr  */
#line 2455 "src/parser_bison.y"
                        {
				(yyvsp[0].expr)->location = (yyloc);
				(yyval.expr) = (yyvsp[0].expr);
			}
#line 10666 "src/parser_bison.c"
    break;

  case 353: /* flowtable_list_expr: flowtable_expr_member  */
#line 2462 "src/parser_bison.y"
                        {
				(yyval.expr) = compound_expr_alloc(&(yyloc), EXPR_LIST);
				compound_expr_add((yyval.expr), (yyvsp[0].expr));
			}
#line 10675 "src/parser_bison.c"
    break;

  case 354: /* flowtable_list_expr: flowtable_list_expr "comma" flowtable_expr_member  */
#line 2467 "src/parser_bison.y"
                        {
				compound_expr_add((yyvsp[-2].expr), (yyvsp[0].expr));
				(yyval.expr) = (yyvsp[-2].expr);
			}
#line 10684 "src/parser_bison.c"
    break;

  case 356: /* flowtable_expr_member: "quoted string"  */
#line 2475 "src/parser_bison.y"
                        {
				struct expr *expr = ifname_expr_alloc(&(yyloc), state->msgs, (yyvsp[0].string));

				if (!expr)
					YYERROR;

				(yyval.expr) = expr;
			}
#line 10697 "src/parser_bison.c"
    break;

  case 357: /* flowtable_expr_member: "string"  */
#line 2484 "src/parser_bison.y"
                        {
				struct expr *expr = ifname_expr_alloc(&(yyloc), state->msgs, (yyvsp[0].string));

				if (!expr)
					YYERROR;

				(yyval.expr) = expr;
			}
#line 10710 "src/parser_bison.c"
    break;

  case 358: /* flowtable_expr_member: variable_expr  */
#line 2493 "src/parser_bison.y"
                        {
				datatype_set((yyvsp[0].expr)->sym->expr, &ifname_type);
				(yyval.expr) = (yyvsp[0].expr);
			}
#line 10719 "src/parser_bison.c"
    break;

  case 359: /* data_type_atom_expr: type_identifier  */
#line 2500 "src/parser_bison.y"
                        {
				const struct datatype *dtype = datatype_lookup_byname((yyvsp[0].string));
				if (dtype == NULL) {
					erec_queue(error(&(yylsp[0]), "unknown datatype %s", (yyvsp[0].string)),
						   state->msgs);
					free_const((yyvsp[0].string));
					YYERROR;
				}
				(yyval.expr) = constant_expr_alloc(&(yylsp[0]), dtype, dtype->byteorder,
							 dtype->size, NULL);
				free_const((yyvsp[0].string));
			}
#line 10736 "src/parser_bison.c"
    break;

  case 360: /* data_type_atom_expr: "time"  */
#line 2513 "src/parser_bison.y"
                        {
				(yyval.expr) = constant_expr_alloc(&(yylsp[0]), &time_type, time_type.byteorder,
							 time_type.size, NULL);
			}
#line 10745 "src/parser_bison.c"
    break;

  case 362: /* data_type_expr: data_type_expr "." data_type_atom_expr  */
#line 2521 "src/parser_bison.y"
                        {
				struct location rhs[] = {
					[1]	= (yylsp[-1]),
					[2]	= (yylsp[0]),
				};

				(yyval.expr) = handle_concat_expr(&(yyloc), (yyval.expr), (yyvsp[-2].expr), (yyvsp[0].expr), rhs);
			}
#line 10758 "src/parser_bison.c"
    break;

  case 363: /* obj_block_alloc: %empty  */
#line 2532 "src/parser_bison.y"
                        {
				(yyval.obj) = obj_alloc(&internal_location);
			}
#line 10766 "src/parser_bison.c"
    break;

  case 364: /* counter_block: %empty  */
#line 2537 "src/parser_bison.y"
                                                { (yyval.obj) = (yyvsp[(-1) - (0)].obj); }
#line 10772 "src/parser_bison.c"
    break;

  case 367: /* counter_block: counter_block counter_config  */
#line 2541 "src/parser_bison.y"
                        {
				(yyval.obj) = (yyvsp[-1].obj);
			}
#line 10780 "src/parser_bison.c"
    break;

  case 368: /* counter_block: counter_block comment_spec  */
#line 2545 "src/parser_bison.y"
                        {
				if (already_set((yyvsp[-1].obj)->comment, &(yylsp[0]), state)) {
					free_const((yyvsp[0].string));
					YYERROR;
				}
				(yyvsp[-1].obj)->comment = (yyvsp[0].string);
			}
#line 10792 "src/parser_bison.c"
    break;

  case 369: /* quota_block: %empty  */
#line 2554 "src/parser_bison.y"
                                                { (yyval.obj) = (yyvsp[(-1) - (0)].obj); }
#line 10798 "src/parser_bison.c"
    break;

  case 372: /* quota_block: quota_block quota_config  */
#line 2558 "src/parser_bison.y"
                        {
				(yyval.obj) = (yyvsp[-1].obj);
			}
#line 10806 "src/parser_bison.c"
    break;

  case 373: /* quota_block: quota_block comment_spec  */
#line 2562 "src/parser_bison.y"
                        {
				if (already_set((yyvsp[-1].obj)->comment, &(yylsp[0]), state)) {
					free_const((yyvsp[0].string));
					YYERROR;
				}
				(yyvsp[-1].obj)->comment = (yyvsp[0].string);
			}
#line 10818 "src/parser_bison.c"
    break;

  case 374: /* ct_helper_block: %empty  */
#line 2571 "src/parser_bison.y"
                                                { (yyval.obj) = (yyvsp[(-1) - (0)].obj); }
#line 10824 "src/parser_bison.c"
    break;

  case 377: /* ct_helper_block: ct_helper_block ct_helper_config  */
#line 2575 "src/parser_bison.y"
                        {
				(yyval.obj) = (yyvsp[-1].obj);
			}
#line 10832 "src/parser_bison.c"
    break;

  case 378: /* ct_helper_block: ct_helper_block comment_spec  */
#line 2579 "src/parser_bison.y"
                        {
				if (already_set((yyvsp[-1].obj)->comment, &(yylsp[0]), state)) {
					free_const((yyvsp[0].string));
					YYERROR;
				}
				(yyvsp[-1].obj)->comment = (yyvsp[0].string);
			}
#line 10844 "src/parser_bison.c"
    break;

  case 379: /* ct_timeout_block: %empty  */
#line 2589 "src/parser_bison.y"
                        {
				(yyval.obj) = (yyvsp[(-1) - (0)].obj);
				init_list_head(&(yyval.obj)->ct_timeout.timeout_list);
				(yyval.obj)->type = NFT_OBJECT_CT_TIMEOUT;
			}
#line 10854 "src/parser_bison.c"
    break;

  case 382: /* ct_timeout_block: ct_timeout_block ct_timeout_config  */
#line 2597 "src/parser_bison.y"
                        {
				(yyval.obj) = (yyvsp[-1].obj);
			}
#line 10862 "src/parser_bison.c"
    break;

  case 383: /* ct_timeout_block: ct_timeout_block comment_spec  */
#line 2601 "src/parser_bison.y"
                        {
				if (already_set((yyvsp[-1].obj)->comment, &(yylsp[0]), state)) {
					free_const((yyvsp[0].string));
					YYERROR;
				}
				(yyvsp[-1].obj)->comment = (yyvsp[0].string);
			}
#line 10874 "src/parser_bison.c"
    break;

  case 384: /* ct_expect_block: %empty  */
#line 2610 "src/parser_bison.y"
                                                { (yyval.obj) = (yyvsp[(-1) - (0)].obj); }
#line 10880 "src/parser_bison.c"
    break;

  case 387: /* ct_expect_block: ct_expect_block ct_expect_config  */
#line 2614 "src/parser_bison.y"
                        {
				(yyval.obj) = (yyvsp[-1].obj);
			}
#line 10888 "src/parser_bison.c"
    break;

  case 388: /* ct_expect_block: ct_expect_block comment_spec  */
#line 2618 "src/parser_bison.y"
                        {
				if (already_set((yyvsp[-1].obj)->comment, &(yylsp[0]), state)) {
					free_const((yyvsp[0].string));
					YYERROR;
				}
				(yyvsp[-1].obj)->comment = (yyvsp[0].string);
			}
#line 10900 "src/parser_bison.c"
    break;

  case 389: /* limit_block: %empty  */
#line 2627 "src/parser_bison.y"
                                                { (yyval.obj) = (yyvsp[(-1) - (0)].obj); }
#line 10906 "src/parser_bison.c"
    break;

  case 392: /* limit_block: limit_block limit_config  */
#line 2631 "src/parser_bison.y"
                        {
				(yyval.obj) = (yyvsp[-1].obj);
			}
#line 10914 "src/parser_bison.c"
    break;

  case 393: /* limit_block: limit_block comment_spec  */
#line 2635 "src/parser_bison.y"
                        {
				if (already_set((yyvsp[-1].obj)->comment, &(yylsp[0]), state)) {
					free_const((yyvsp[0].string));
					YYERROR;
				}
				(yyvsp[-1].obj)->comment = (yyvsp[0].string);
			}
#line 10926 "src/parser_bison.c"
    break;

  case 394: /* secmark_block: %empty  */
#line 2644 "src/parser_bison.y"
                                                { (yyval.obj) = (yyvsp[(-1) - (0)].obj); }
#line 10932 "src/parser_bison.c"
    break;

  case 397: /* secmark_block: secmark_block secmark_config  */
#line 2648 "src/parser_bison.y"
                        {
				(yyval.obj) = (yyvsp[-1].obj);
			}
#line 10940 "src/parser_bison.c"
    break;

  case 398: /* secmark_block: secmark_block comment_spec  */
#line 2652 "src/parser_bison.y"
                        {
				if (already_set((yyvsp[-1].obj)->comment, &(yylsp[0]), state)) {
					free_const((yyvsp[0].string));
					YYERROR;
				}
				(yyvsp[-1].obj)->comment = (yyvsp[0].string);
			}
#line 10952 "src/parser_bison.c"
    break;

  case 399: /* synproxy_block: %empty  */
#line 2661 "src/parser_bison.y"
                                                { (yyval.obj) = (yyvsp[(-1) - (0)].obj); }
#line 10958 "src/parser_bison.c"
    break;

  case 402: /* synproxy_block: synproxy_block synproxy_config  */
#line 2665 "src/parser_bison.y"
                        {
				(yyval.obj) = (yyvsp[-1].obj);
			}
#line 10966 "src/parser_bison.c"
    break;

  case 403: /* synproxy_block: synproxy_block comment_spec  */
#line 2669 "src/parser_bison.y"
                        {
				if (already_set((yyvsp[-1].obj)->comment, &(yylsp[0]), state)) {
					free_const((yyvsp[0].string));
					YYERROR;
				}
				(yyvsp[-1].obj)->comment = (yyvsp[0].string);
			}
#line 10978 "src/parser_bison.c"
    break;

  case 404: /* type_identifier: "string"  */
#line 2678 "src/parser_bison.y"
                                        { (yyval.string) = (yyvsp[0].string); }
#line 10984 "src/parser_bison.c"
    break;

  case 405: /* type_identifier: "mark"  */
#line 2679 "src/parser_bison.y"
                                        { (yyval.string) = xstrdup("mark"); }
#line 10990 "src/parser_bison.c"
    break;

  case 406: /* type_identifier: "dscp"  */
#line 2680 "src/parser_bison.y"
                                        { (yyval.string) = xstrdup("dscp"); }
#line 10996 "src/parser_bison.c"
    break;

  case 407: /* type_identifier: "ecn"  */
#line 2681 "src/parser_bison.y"
                                        { (yyval.string) = xstrdup("ecn"); }
#line 11002 "src/parser_bison.c"
    break;

  case 408: /* type_identifier: "classid"  */
#line 2682 "src/parser_bison.y"
                                        { (yyval.string) = xstrdup("classid"); }
#line 11008 "src/parser_bison.c"
    break;

  case 409: /* hook_spec: "type" close_scope_type "string" "hook" "string" dev_spec prio_spec  */
#line 2686 "src/parser_bison.y"
                        {
				const char *chain_type = chain_type_name_lookup((yyvsp[-4].string));

				if (chain_type == NULL) {
					erec_queue(error(&(yylsp[-4]), "unknown chain type"),
						   state->msgs);
					free_const((yyvsp[-4].string));
					free_const((yyvsp[-2].string));
					expr_free((yyvsp[-1].expr));
					expr_free((yyvsp[0].prio_spec).expr);
					YYERROR;
				}
				(yyvsp[-7].chain)->type.loc = (yylsp[-4]);
				(yyvsp[-7].chain)->type.str = xstrdup(chain_type);
				free_const((yyvsp[-4].string));

				(yyvsp[-7].chain)->loc = (yyloc);
				(yyvsp[-7].chain)->hook.loc = (yylsp[-2]);
				(yyvsp[-7].chain)->hook.name = chain_hookname_lookup((yyvsp[-2].string));
				if ((yyvsp[-7].chain)->hook.name == NULL) {
					erec_queue(error(&(yylsp[-2]), "unknown chain hook"),
						   state->msgs);
					free_const((yyvsp[-2].string));
					expr_free((yyvsp[-1].expr));
					expr_free((yyvsp[0].prio_spec).expr);
					YYERROR;
				}
				free_const((yyvsp[-2].string));

				(yyvsp[-7].chain)->dev_expr	= (yyvsp[-1].expr);
				(yyvsp[-7].chain)->priority	= (yyvsp[0].prio_spec);
				(yyvsp[-7].chain)->flags	|= CHAIN_F_BASECHAIN;
			}
#line 11046 "src/parser_bison.c"
    break;

  case 410: /* prio_spec: "priority" extended_prio_spec  */
#line 2722 "src/parser_bison.y"
                        {
				(yyval.prio_spec) = (yyvsp[0].prio_spec);
				(yyval.prio_spec).loc = (yyloc);
			}
#line 11055 "src/parser_bison.c"
    break;

  case 411: /* extended_prio_name: "out"  */
#line 2729 "src/parser_bison.y"
                        {
				(yyval.string) = strdup("out");
			}
#line 11063 "src/parser_bison.c"
    break;

  case 413: /* extended_prio_spec: int_num  */
#line 2736 "src/parser_bison.y"
                        {
				struct prio_spec spec = {0};

				spec.expr = constant_expr_alloc(&(yyloc), &integer_type,
								BYTEORDER_HOST_ENDIAN,
								sizeof(int) *
								BITS_PER_BYTE, &(yyvsp[0].val32));
				(yyval.prio_spec) = spec;
			}
#line 11077 "src/parser_bison.c"
    break;

  case 414: /* extended_prio_spec: variable_expr  */
#line 2746 "src/parser_bison.y"
                        {
				struct prio_spec spec = {0};

				spec.expr = (yyvsp[0].expr);
				(yyval.prio_spec) = spec;
			}
#line 11088 "src/parser_bison.c"
    break;

  case 415: /* extended_prio_spec: extended_prio_name  */
#line 2753 "src/parser_bison.y"
                        {
				struct prio_spec spec = {0};

				spec.expr = constant_expr_alloc(&(yyloc), &string_type,
								BYTEORDER_HOST_ENDIAN,
								strlen((yyvsp[0].string)) * BITS_PER_BYTE,
								(yyvsp[0].string));
				free_const((yyvsp[0].string));
				(yyval.prio_spec) = spec;
			}
#line 11103 "src/parser_bison.c"
    break;

  case 416: /* extended_prio_spec: extended_prio_name "+" "number"  */
#line 2764 "src/parser_bison.y"
                        {
				struct prio_spec spec = {0};

				char str[NFT_NAME_MAXLEN];
				snprintf(str, sizeof(str), "%s + %" PRIu64, (yyvsp[-2].string), (yyvsp[0].val));
				spec.expr = constant_expr_alloc(&(yyloc), &string_type,
								BYTEORDER_HOST_ENDIAN,
								strlen(str) * BITS_PER_BYTE,
								str);
				free_const((yyvsp[-2].string));
				(yyval.prio_spec) = spec;
			}
#line 11120 "src/parser_bison.c"
    break;

  case 417: /* extended_prio_spec: extended_prio_name "-" "number"  */
#line 2777 "src/parser_bison.y"
                        {
				struct prio_spec spec = {0};
				char str[NFT_NAME_MAXLEN];

				snprintf(str, sizeof(str), "%s - %" PRIu64, (yyvsp[-2].string), (yyvsp[0].val));
				spec.expr = constant_expr_alloc(&(yyloc), &string_type,
								BYTEORDER_HOST_ENDIAN,
								strlen(str) * BITS_PER_BYTE,
								str);
				free_const((yyvsp[-2].string));
				(yyval.prio_spec) = spec;
			}
#line 11137 "src/parser_bison.c"
    break;

  case 418: /* int_num: "number"  */
#line 2791 "src/parser_bison.y"
                                                        { (yyval.val32) = (yyvsp[0].val); }
#line 11143 "src/parser_bison.c"
    break;

  case 419: /* int_num: "-" "number"  */
#line 2792 "src/parser_bison.y"
                                                        { (yyval.val32) = -(yyvsp[0].val); }
#line 11149 "src/parser_bison.c"
    break;

  case 420: /* dev_spec: "device" string  */
#line 2796 "src/parser_bison.y"
                        {
				struct expr *expr = ifname_expr_alloc(&(yyloc), state->msgs, (yyvsp[0].string));

				if (!expr)
					YYERROR;

				(yyval.expr) = compound_expr_alloc(&(yyloc), EXPR_LIST);
				compound_expr_add((yyval.expr), expr);

			}
#line 11164 "src/parser_bison.c"
    break;

  case 421: /* dev_spec: "device" variable_expr  */
#line 2807 "src/parser_bison.y"
                        {
				datatype_set((yyvsp[0].expr)->sym->expr, &ifname_type);
				(yyval.expr) = compound_expr_alloc(&(yyloc), EXPR_LIST);
				compound_expr_add((yyval.expr), (yyvsp[0].expr));
			}
#line 11174 "src/parser_bison.c"
    break;

  case 422: /* dev_spec: "devices" '=' flowtable_expr  */
#line 2813 "src/parser_bison.y"
                        {
				(yyval.expr) = (yyvsp[0].expr);
			}
#line 11182 "src/parser_bison.c"
    break;

  case 423: /* dev_spec: %empty  */
#line 2816 "src/parser_bison.y"
                                                        { (yyval.expr) = NULL; }
#line 11188 "src/parser_bison.c"
    break;

  case 424: /* flags_spec: "flags" "offload"  */
#line 2820 "src/parser_bison.y"
                        {
				(yyvsp[-2].chain)->flags |= CHAIN_F_HW_OFFLOAD;
			}
#line 11196 "src/parser_bison.c"
    break;

  case 425: /* policy_spec: "policy" policy_expr close_scope_policy  */
#line 2826 "src/parser_bison.y"
                        {
				if ((yyvsp[-3].chain)->policy) {
					erec_queue(error(&(yyloc), "you cannot set chain policy twice"),
						   state->msgs);
					expr_free((yyvsp[-1].expr));
					YYERROR;
				}
				(yyvsp[-3].chain)->policy		= (yyvsp[-1].expr);
				(yyvsp[-3].chain)->policy->location	= (yyloc);
			}
#line 11211 "src/parser_bison.c"
    break;

  case 426: /* policy_expr: variable_expr  */
#line 2839 "src/parser_bison.y"
                        {
				datatype_set((yyvsp[0].expr)->sym->expr, &policy_type);
				(yyval.expr) = (yyvsp[0].expr);
			}
#line 11220 "src/parser_bison.c"
    break;

  case 427: /* policy_expr: chain_policy  */
#line 2844 "src/parser_bison.y"
                        {
				(yyval.expr) = constant_expr_alloc(&(yyloc), &integer_type,
							 BYTEORDER_HOST_ENDIAN,
							 sizeof(int) *
							 BITS_PER_BYTE, &(yyvsp[0].val32));
			}
#line 11231 "src/parser_bison.c"
    break;

  case 428: /* chain_policy: "accept"  */
#line 2852 "src/parser_bison.y"
                                                { (yyval.val32) = NF_ACCEPT; }
#line 11237 "src/parser_bison.c"
    break;

  case 429: /* chain_policy: "drop"  */
#line 2853 "src/parser_bison.y"
                                                { (yyval.val32) = NF_DROP;   }
#line 11243 "src/parser_bison.c"
    break;

  case 431: /* identifier: "last"  */
#line 2857 "src/parser_bison.y"
                                                { (yyval.string) = xstrdup("last"); }
#line 11249 "src/parser_bison.c"
    break;

  case 435: /* time_spec: "string"  */
#line 2866 "src/parser_bison.y"
                        {
				struct error_record *erec;
				uint64_t res;

				erec = time_parse(&(yylsp[0]), (yyvsp[0].string), &res);
				free_const((yyvsp[0].string));
				if (erec != NULL) {
					erec_queue(erec, state->msgs);
					YYERROR;
				}
				(yyval.val) = res;
			}
#line 11266 "src/parser_bison.c"
    break;

  case 437: /* time_spec_or_num_s: time_spec  */
#line 2882 "src/parser_bison.y"
                                          { (yyval.val) = (yyvsp[0].val) / 1000u; }
#line 11272 "src/parser_bison.c"
    break;

  case 438: /* family_spec: %empty  */
#line 2885 "src/parser_bison.y"
                                                        { (yyval.val) = NFPROTO_IPV4; }
#line 11278 "src/parser_bison.c"
    break;

  case 440: /* family_spec_explicit: "ip" close_scope_ip  */
#line 2889 "src/parser_bison.y"
                                                        { (yyval.val) = NFPROTO_IPV4; }
#line 11284 "src/parser_bison.c"
    break;

  case 441: /* family_spec_explicit: "ip6" close_scope_ip6  */
#line 2890 "src/parser_bison.y"
                                                        { (yyval.val) = NFPROTO_IPV6; }
#line 11290 "src/parser_bison.c"
    break;

  case 442: /* family_spec_explicit: "inet"  */
#line 2891 "src/parser_bison.y"
                                                        { (yyval.val) = NFPROTO_INET; }
#line 11296 "src/parser_bison.c"
    break;

  case 443: /* family_spec_explicit: "arp" close_scope_arp  */
#line 2892 "src/parser_bison.y"
                                                        { (yyval.val) = NFPROTO_ARP; }
#line 11302 "src/parser_bison.c"
    break;

  case 444: /* family_spec_explicit: "bridge"  */
#line 2893 "src/parser_bison.y"
                                                        { (yyval.val) = NFPROTO_BRIDGE; }
#line 11308 "src/parser_bison.c"
    break;

  case 445: /* family_spec_explicit: "netdev"  */
#line 2894 "src/parser_bison.y"
                                                        { (yyval.val) = NFPROTO_NETDEV; }
#line 11314 "src/parser_bison.c"
    break;

  case 446: /* table_spec: family_spec identifier  */
#line 2898 "src/parser_bison.y"
                        {
				memset(&(yyval.handle), 0, sizeof((yyval.handle)));
				(yyval.handle).family	= (yyvsp[-1].val);
				(yyval.handle).table.location = (yylsp[0]);
				(yyval.handle).table.name	= (yyvsp[0].string);
			}
#line 11325 "src/parser_bison.c"
    break;

  case 447: /* tableid_spec: family_spec "handle" "number"  */
#line 2907 "src/parser_bison.y"
                        {
				memset(&(yyval.handle), 0, sizeof((yyval.handle)));
				(yyval.handle).family 		= (yyvsp[-2].val);
				(yyval.handle).handle.id 		= (yyvsp[0].val);
				(yyval.handle).handle.location	= (yylsp[0]);
			}
#line 11336 "src/parser_bison.c"
    break;

  case 448: /* chain_spec: table_spec identifier  */
#line 2916 "src/parser_bison.y"
                        {
				(yyval.handle)		= (yyvsp[-1].handle);
				(yyval.handle).chain.name	= (yyvsp[0].string);
				(yyval.handle).chain.location = (yylsp[0]);
			}
#line 11346 "src/parser_bison.c"
    break;

  case 449: /* chainid_spec: table_spec "handle" "number"  */
#line 2924 "src/parser_bison.y"
                        {
				(yyval.handle) 			= (yyvsp[-2].handle);
				(yyval.handle).handle.location 	= (yylsp[0]);
				(yyval.handle).handle.id 		= (yyvsp[0].val);
			}
#line 11356 "src/parser_bison.c"
    break;

  case 450: /* chain_identifier: identifier  */
#line 2932 "src/parser_bison.y"
                        {
				memset(&(yyval.handle), 0, sizeof((yyval.handle)));
				(yyval.handle).chain.name		= (yyvsp[0].string);
				(yyval.handle).chain.location	= (yylsp[0]);
			}
#line 11366 "src/parser_bison.c"
    break;

  case 451: /* set_spec: table_spec identifier  */
#line 2940 "src/parser_bison.y"
                        {
				(yyval.handle)		= (yyvsp[-1].handle);
				(yyval.handle).set.name	= (yyvsp[0].string);
				(yyval.handle).set.location	= (yylsp[0]);
			}
#line 11376 "src/parser_bison.c"
    break;

  case 452: /* setid_spec: table_spec "handle" "number"  */
#line 2948 "src/parser_bison.y"
                        {
				(yyval.handle) 			= (yyvsp[-2].handle);
				(yyval.handle).handle.location 	= (yylsp[0]);
				(yyval.handle).handle.id 		= (yyvsp[0].val);
			}
#line 11386 "src/parser_bison.c"
    break;

  case 453: /* set_identifier: identifier  */
#line 2956 "src/parser_bison.y"
                        {
				memset(&(yyval.handle), 0, sizeof((yyval.handle)));
				(yyval.handle).set.name	= (yyvsp[0].string);
				(yyval.handle).set.location	= (yylsp[0]);
			}
#line 11396 "src/parser_bison.c"
    break;

  case 454: /* flowtable_spec: table_spec identifier  */
#line 2964 "src/parser_bison.y"
                        {
				(yyval.handle)			= (yyvsp[-1].handle);
				(yyval.handle).flowtable.name	= (yyvsp[0].string);
				(yyval.handle).flowtable.location	= (yylsp[0]);
			}
#line 11406 "src/parser_bison.c"
    break;

  case 455: /* flowtableid_spec: table_spec "handle" "number"  */
#line 2972 "src/parser_bison.y"
                        {
				(yyval.handle)			= (yyvsp[-2].handle);
				(yyval.handle).handle.location	= (yylsp[0]);
				(yyval.handle).handle.id		= (yyvsp[0].val);
			}
#line 11416 "src/parser_bison.c"
    break;

  case 456: /* flowtable_identifier: identifier  */
#line 2980 "src/parser_bison.y"
                        {
				memset(&(yyval.handle), 0, sizeof((yyval.handle)));
				(yyval.handle).flowtable.name	= (yyvsp[0].string);
				(yyval.handle).flowtable.location	= (yylsp[0]);
			}
#line 11426 "src/parser_bison.c"
    break;

  case 457: /* obj_spec: table_spec identifier  */
#line 2988 "src/parser_bison.y"
                        {
				(yyval.handle)		= (yyvsp[-1].handle);
				(yyval.handle).obj.name	= (yyvsp[0].string);
				(yyval.handle).obj.location	= (yylsp[0]);
			}
#line 11436 "src/parser_bison.c"
    break;

  case 458: /* objid_spec: table_spec "handle" "number"  */
#line 2996 "src/parser_bison.y"
                        {
				(yyval.handle) 			= (yyvsp[-2].handle);
				(yyval.handle).handle.location	= (yylsp[0]);
				(yyval.handle).handle.id		= (yyvsp[0].val);
			}
#line 11446 "src/parser_bison.c"
    break;

  case 459: /* obj_identifier: identifier  */
#line 3004 "src/parser_bison.y"
                        {
				memset(&(yyval.handle), 0, sizeof((yyval.handle)));
				(yyval.handle).obj.name		= (yyvsp[0].string);
				(yyval.handle).obj.location		= (yylsp[0]);
			}
#line 11456 "src/parser_bison.c"
    break;

  case 460: /* handle_spec: "handle" "number"  */
#line 3012 "src/parser_bison.y"
                        {
				memset(&(yyval.handle), 0, sizeof((yyval.handle)));
				(yyval.handle).handle.location	= (yylsp[0]);
				(yyval.handle).handle.id		= (yyvsp[0].val);
			}
#line 11466 "src/parser_bison.c"
    break;

  case 461: /* position_spec: "position" "number"  */
#line 3020 "src/parser_bison.y"
                        {
				memset(&(yyval.handle), 0, sizeof((yyval.handle)));
				(yyval.handle).position.location	= (yyloc);
				(yyval.handle).position.id		= (yyvsp[0].val);
			}
#line 11476 "src/parser_bison.c"
    break;

  case 462: /* index_spec: "index" "number"  */
#line 3028 "src/parser_bison.y"
                        {
				memset(&(yyval.handle), 0, sizeof((yyval.handle)));
				(yyval.handle).index.location	= (yyloc);
				(yyval.handle).index.id		= (yyvsp[0].val) + 1;
			}
#line 11486 "src/parser_bison.c"
    break;

  case 463: /* rule_position: chain_spec  */
#line 3036 "src/parser_bison.y"
                        {
				(yyval.handle) = (yyvsp[0].handle);
			}
#line 11494 "src/parser_bison.c"
    break;

  case 464: /* rule_position: chain_spec position_spec  */
#line 3040 "src/parser_bison.y"
                        {
				handle_merge(&(yyvsp[-1].handle), &(yyvsp[0].handle));
				(yyval.handle) = (yyvsp[-1].handle);
			}
#line 11503 "src/parser_bison.c"
    break;

  case 465: /* rule_position: chain_spec handle_spec  */
#line 3045 "src/parser_bison.y"
                        {
				(yyvsp[0].handle).position.location = (yyvsp[0].handle).handle.location;
				(yyvsp[0].handle).position.id = (yyvsp[0].handle).handle.id;
				(yyvsp[0].handle).handle.id = 0;
				handle_merge(&(yyvsp[-1].handle), &(yyvsp[0].handle));
				(yyval.handle) = (yyvsp[-1].handle);
			}
#line 11515 "src/parser_bison.c"
    break;

  case 466: /* rule_position: chain_spec index_spec  */
#line 3053 "src/parser_bison.y"
                        {
				handle_merge(&(yyvsp[-1].handle), &(yyvsp[0].handle));
				(yyval.handle) = (yyvsp[-1].handle);
			}
#line 11524 "src/parser_bison.c"
    break;

  case 467: /* ruleid_spec: chain_spec handle_spec  */
#line 3060 "src/parser_bison.y"
                        {
				handle_merge(&(yyvsp[-1].handle), &(yyvsp[0].handle));
				(yyval.handle) = (yyvsp[-1].handle);
			}
#line 11533 "src/parser_bison.c"
    break;

  case 468: /* comment_spec: "comment" string  */
#line 3067 "src/parser_bison.y"
                        {
				if (strlen((yyvsp[0].string)) > NFTNL_UDATA_COMMENT_MAXLEN) {
					erec_queue(error(&(yylsp[0]), "comment too long, %d characters maximum allowed",
							 NFTNL_UDATA_COMMENT_MAXLEN),
						   state->msgs);
					free_const((yyvsp[0].string));
					YYERROR;
				}
				(yyval.string) = (yyvsp[0].string);
			}
#line 11548 "src/parser_bison.c"
    break;

  case 469: /* ruleset_spec: %empty  */
#line 3080 "src/parser_bison.y"
                        {
				memset(&(yyval.handle), 0, sizeof((yyval.handle)));
				(yyval.handle).family	= NFPROTO_UNSPEC;
			}
#line 11557 "src/parser_bison.c"
    break;

  case 470: /* ruleset_spec: family_spec_explicit  */
#line 3085 "src/parser_bison.y"
                        {
				memset(&(yyval.handle), 0, sizeof((yyval.handle)));
				(yyval.handle).family	= (yyvsp[0].val);
			}
#line 11566 "src/parser_bison.c"
    break;

  case 471: /* rule: rule_alloc  */
#line 3092 "src/parser_bison.y"
                        {
				(yyval.rule)->comment = NULL;
			}
#line 11574 "src/parser_bison.c"
    break;

  case 472: /* rule: rule_alloc comment_spec  */
#line 3096 "src/parser_bison.y"
                        {
				(yyval.rule)->comment = (yyvsp[0].string);
			}
#line 11582 "src/parser_bison.c"
    break;

  case 473: /* rule_alloc: stmt_list  */
#line 3102 "src/parser_bison.y"
                        {
				struct stmt *i;

				(yyval.rule) = rule_alloc(&(yyloc), NULL);
				list_for_each_entry(i, (yyvsp[0].list), list)
					(yyval.rule)->num_stmts++;
				list_splice_tail((yyvsp[0].list), &(yyval.rule)->stmts);
				free((yyvsp[0].list));
			}
#line 11596 "src/parser_bison.c"
    break;

  case 474: /* stmt_list: stmt  */
#line 3114 "src/parser_bison.y"
                        {
				(yyval.list) = xmalloc(sizeof(*(yyval.list)));
				init_list_head((yyval.list));
				list_add_tail(&(yyvsp[0].stmt)->list, (yyval.list));
			}
#line 11606 "src/parser_bison.c"
    break;

  case 475: /* stmt_list: stmt_list stmt  */
#line 3120 "src/parser_bison.y"
                        {
				(yyval.list) = (yyvsp[-1].list);
				list_add_tail(&(yyvsp[0].stmt)->list, (yyvsp[-1].list));
			}
#line 11615 "src/parser_bison.c"
    break;

  case 476: /* stateful_stmt_list: stateful_stmt  */
#line 3127 "src/parser_bison.y"
                        {
				(yyval.list) = xmalloc(sizeof(*(yyval.list)));
				init_list_head((yyval.list));
				list_add_tail(&(yyvsp[0].stmt)->list, (yyval.list));
			}
#line 11625 "src/parser_bison.c"
    break;

  case 477: /* stateful_stmt_list: stateful_stmt_list stateful_stmt  */
#line 3133 "src/parser_bison.y"
                        {
				(yyval.list) = (yyvsp[-1].list);
				list_add_tail(&(yyvsp[0].stmt)->list, (yyvsp[-1].list));
			}
#line 11634 "src/parser_bison.c"
    break;

  case 478: /* objref_stmt_counter: "counter" "name" stmt_expr close_scope_counter  */
#line 3140 "src/parser_bison.y"
                        {
				(yyval.stmt) = objref_stmt_alloc(&(yyloc));
				(yyval.stmt)->objref.type = NFT_OBJECT_COUNTER;
				(yyval.stmt)->objref.expr = (yyvsp[-1].expr);
			}
#line 11644 "src/parser_bison.c"
    break;

  case 479: /* objref_stmt_limit: "limit" "name" stmt_expr close_scope_limit  */
#line 3148 "src/parser_bison.y"
                        {
				(yyval.stmt) = objref_stmt_alloc(&(yyloc));
				(yyval.stmt)->objref.type = NFT_OBJECT_LIMIT;
				(yyval.stmt)->objref.expr = (yyvsp[-1].expr);
			}
#line 11654 "src/parser_bison.c"
    break;

  case 480: /* objref_stmt_quota: "quota" "name" stmt_expr close_scope_quota  */
#line 3156 "src/parser_bison.y"
                        {
				(yyval.stmt) = objref_stmt_alloc(&(yyloc));
				(yyval.stmt)->objref.type = NFT_OBJECT_QUOTA;
				(yyval.stmt)->objref.expr = (yyvsp[-1].expr);
			}
#line 11664 "src/parser_bison.c"
    break;

  case 481: /* objref_stmt_synproxy: "synproxy" "name" stmt_expr close_scope_synproxy  */
#line 3164 "src/parser_bison.y"
                        {
				(yyval.stmt) = objref_stmt_alloc(&(yyloc));
				(yyval.stmt)->objref.type = NFT_OBJECT_SYNPROXY;
				(yyval.stmt)->objref.expr = (yyvsp[-1].expr);
			}
#line 11674 "src/parser_bison.c"
    break;

  case 482: /* objref_stmt_ct: "ct" "timeout" "set" stmt_expr close_scope_ct  */
#line 3172 "src/parser_bison.y"
                        {
				(yyval.stmt) = objref_stmt_alloc(&(yyloc));
				(yyval.stmt)->objref.type = NFT_OBJECT_CT_TIMEOUT;
				(yyval.stmt)->objref.expr = (yyvsp[-1].expr);

			}
#line 11685 "src/parser_bison.c"
    break;

  case 483: /* objref_stmt_ct: "ct" "expectation" "set" stmt_expr close_scope_ct  */
#line 3179 "src/parser_bison.y"
                        {
				(yyval.stmt) = objref_stmt_alloc(&(yyloc));
				(yyval.stmt)->objref.type = NFT_OBJECT_CT_EXPECT;
				(yyval.stmt)->objref.expr = (yyvsp[-1].expr);
			}
#line 11695 "src/parser_bison.c"
    break;

  case 517: /* xt_stmt: "xt" "string" string  */
#line 3226 "src/parser_bison.y"
                        {
				(yyval.stmt) = NULL;
				free_const((yyvsp[-1].string));
				free_const((yyvsp[0].string));
				erec_queue(error(&(yyloc), "unsupported xtables compat expression, use iptables-nft with this ruleset"),
					   state->msgs);
				YYERROR;
			}
#line 11708 "src/parser_bison.c"
    break;

  case 518: /* chain_stmt_type: "jump"  */
#line 3236 "src/parser_bison.y"
                                        { (yyval.val) = NFT_JUMP; }
#line 11714 "src/parser_bison.c"
    break;

  case 519: /* chain_stmt_type: "goto"  */
#line 3237 "src/parser_bison.y"
                                        { (yyval.val) = NFT_GOTO; }
#line 11720 "src/parser_bison.c"
    break;

  case 520: /* chain_stmt: chain_stmt_type chain_block_alloc '{' subchain_block '}'  */
#line 3241 "src/parser_bison.y"
                        {
				(yyvsp[-3].chain)->location = (yylsp[-3]);
				close_scope(state);
				(yyvsp[-1].chain)->location = (yylsp[-1]);
				(yyval.stmt) = chain_stmt_alloc(&(yyloc), (yyvsp[-1].chain), (yyvsp[-4].val));
			}
#line 11731 "src/parser_bison.c"
    break;

  case 521: /* verdict_stmt: verdict_expr  */
#line 3250 "src/parser_bison.y"
                        {
				(yyval.stmt) = verdict_stmt_alloc(&(yyloc), (yyvsp[0].expr));
			}
#line 11739 "src/parser_bison.c"
    break;

  case 522: /* verdict_stmt: verdict_map_stmt  */
#line 3254 "src/parser_bison.y"
                        {
				(yyval.stmt) = verdict_stmt_alloc(&(yyloc), (yyvsp[0].expr));
			}
#line 11747 "src/parser_bison.c"
    break;

  case 523: /* verdict_map_stmt: concat_expr "vmap" verdict_map_expr  */
#line 3260 "src/parser_bison.y"
                        {
				(yyval.expr) = map_expr_alloc(&(yyloc), (yyvsp[-2].expr), (yyvsp[0].expr));
			}
#line 11755 "src/parser_bison.c"
    break;

  case 524: /* verdict_map_expr: '{' verdict_map_list_expr '}'  */
#line 3266 "src/parser_bison.y"
                        {
				(yyvsp[-1].expr)->location = (yyloc);
				(yyval.expr) = (yyvsp[-1].expr);
			}
#line 11764 "src/parser_bison.c"
    break;

  case 526: /* verdict_map_list_expr: verdict_map_list_member_expr  */
#line 3274 "src/parser_bison.y"
                        {
				(yyval.expr) = set_expr_alloc(&(yyloc), NULL);
				compound_expr_add((yyval.expr), (yyvsp[0].expr));
			}
#line 11773 "src/parser_bison.c"
    break;

  case 527: /* verdict_map_list_expr: verdict_map_list_expr "comma" verdict_map_list_member_expr  */
#line 3279 "src/parser_bison.y"
                        {
				compound_expr_add((yyvsp[-2].expr), (yyvsp[0].expr));
				(yyval.expr) = (yyvsp[-2].expr);
			}
#line 11782 "src/parser_bison.c"
    break;

  case 529: /* verdict_map_list_member_expr: opt_newline set_elem_expr "colon" verdict_expr opt_newline  */
#line 3287 "src/parser_bison.y"
                        {
				(yyval.expr) = mapping_expr_alloc(&(yylsp[-3]), (yyvsp[-3].expr), (yyvsp[-1].expr));
			}
#line 11790 "src/parser_bison.c"
    break;

  case 530: /* ct_limit_stmt_alloc: "ct" "count"  */
#line 3293 "src/parser_bison.y"
                        {
				(yyval.stmt) = connlimit_stmt_alloc(&(yyloc));
			}
#line 11798 "src/parser_bison.c"
    break;

  case 532: /* ct_limit_args: "number"  */
#line 3302 "src/parser_bison.y"
                        {
				assert((yyvsp[-1].stmt)->type == STMT_CONNLIMIT);

				(yyvsp[-1].stmt)->connlimit.count	= (yyvsp[0].val);
			}
#line 11808 "src/parser_bison.c"
    break;

  case 533: /* ct_limit_args: "over" "number"  */
#line 3308 "src/parser_bison.y"
                        {
				assert((yyvsp[-2].stmt)->type == STMT_CONNLIMIT);

				(yyvsp[-2].stmt)->connlimit.count = (yyvsp[0].val);
				(yyvsp[-2].stmt)->connlimit.flags = NFT_CONNLIMIT_F_INV;
			}
#line 11819 "src/parser_bison.c"
    break;

  case 536: /* counter_stmt_alloc: "counter"  */
#line 3320 "src/parser_bison.y"
                        {
				(yyval.stmt) = counter_stmt_alloc(&(yyloc));
			}
#line 11827 "src/parser_bison.c"
    break;

  case 537: /* counter_args: counter_arg  */
#line 3326 "src/parser_bison.y"
                        {
				(yyval.stmt)	= (yyvsp[-1].stmt);
			}
#line 11835 "src/parser_bison.c"
    break;

  case 539: /* counter_arg: "packets" "number"  */
#line 3333 "src/parser_bison.y"
                        {
				assert((yyvsp[-2].stmt)->type == STMT_COUNTER);
				(yyvsp[-2].stmt)->counter.packets = (yyvsp[0].val);
			}
#line 11844 "src/parser_bison.c"
    break;

  case 540: /* counter_arg: "bytes" "number"  */
#line 3338 "src/parser_bison.y"
                        {
				assert((yyvsp[-2].stmt)->type == STMT_COUNTER);
				(yyvsp[-2].stmt)->counter.bytes	 = (yyvsp[0].val);
			}
#line 11853 "src/parser_bison.c"
    break;

  case 541: /* last_stmt_alloc: "last"  */
#line 3345 "src/parser_bison.y"
                        {
				(yyval.stmt) = last_stmt_alloc(&(yyloc));
			}
#line 11861 "src/parser_bison.c"
    break;

  case 545: /* last_args: "used" time_spec  */
#line 3356 "src/parser_bison.y"
                        {
				struct last_stmt *last;

				assert((yyvsp[-2].stmt)->type == STMT_LAST);
				last = &(yyvsp[-2].stmt)->last;
				last->used = (yyvsp[0].val);
				last->set = true;
			}
#line 11874 "src/parser_bison.c"
    break;

  case 548: /* log_stmt_alloc: "log"  */
#line 3371 "src/parser_bison.y"
                        {
				(yyval.stmt) = log_stmt_alloc(&(yyloc));
			}
#line 11882 "src/parser_bison.c"
    break;

  case 549: /* log_args: log_arg  */
#line 3377 "src/parser_bison.y"
                        {
				(yyval.stmt)	= (yyvsp[-1].stmt);
			}
#line 11890 "src/parser_bison.c"
    break;

  case 551: /* log_arg: "prefix" string  */
#line 3384 "src/parser_bison.y"
                        {
				struct scope *scope = current_scope(state);
				struct error_record *erec;
				const char *prefix;

				prefix = str_preprocess(state, &(yylsp[0]), scope, (yyvsp[0].string), &erec);
				if (!prefix) {
					erec_queue(erec, state->msgs);
					free_const((yyvsp[0].string));
					YYERROR;
				}

				free_const((yyvsp[0].string));
				(yyvsp[-2].stmt)->log.prefix = prefix;
				(yyvsp[-2].stmt)->log.flags |= STMT_LOG_PREFIX;
			}
#line 11911 "src/parser_bison.c"
    break;

  case 552: /* log_arg: "group" "number"  */
#line 3401 "src/parser_bison.y"
                        {
				(yyvsp[-2].stmt)->log.group	 = (yyvsp[0].val);
				(yyvsp[-2].stmt)->log.flags 	|= STMT_LOG_GROUP;
			}
#line 11920 "src/parser_bison.c"
    break;

  case 553: /* log_arg: "snaplen" "number"  */
#line 3406 "src/parser_bison.y"
                        {
				(yyvsp[-2].stmt)->log.snaplen	 = (yyvsp[0].val);
				(yyvsp[-2].stmt)->log.flags 	|= STMT_LOG_SNAPLEN;
			}
#line 11929 "src/parser_bison.c"
    break;

  case 554: /* log_arg: "queue-threshold" "number"  */
#line 3411 "src/parser_bison.y"
                        {
				(yyvsp[-2].stmt)->log.qthreshold = (yyvsp[0].val);
				(yyvsp[-2].stmt)->log.flags 	|= STMT_LOG_QTHRESHOLD;
			}
#line 11938 "src/parser_bison.c"
    break;

  case 555: /* log_arg: "level" level_type  */
#line 3416 "src/parser_bison.y"
                        {
				(yyvsp[-2].stmt)->log.level	= (yyvsp[0].val);
				(yyvsp[-2].stmt)->log.flags 	|= STMT_LOG_LEVEL;
			}
#line 11947 "src/parser_bison.c"
    break;

  case 556: /* log_arg: "flags" log_flags  */
#line 3421 "src/parser_bison.y"
                        {
				(yyvsp[-2].stmt)->log.logflags	|= (yyvsp[0].val);
			}
#line 11955 "src/parser_bison.c"
    break;

  case 557: /* level_type: string  */
#line 3427 "src/parser_bison.y"
                        {
				if (!strcmp("emerg", (yyvsp[0].string)))
					(yyval.val) = NFT_LOGLEVEL_EMERG;
				else if (!strcmp("alert", (yyvsp[0].string)))
					(yyval.val) = NFT_LOGLEVEL_ALERT;
				else if (!strcmp("crit", (yyvsp[0].string)))
					(yyval.val) = NFT_LOGLEVEL_CRIT;
				else if (!strcmp("err", (yyvsp[0].string)))
					(yyval.val) = NFT_LOGLEVEL_ERR;
				else if (!strcmp("warn", (yyvsp[0].string)))
					(yyval.val) = NFT_LOGLEVEL_WARNING;
				else if (!strcmp("notice", (yyvsp[0].string)))
					(yyval.val) = NFT_LOGLEVEL_NOTICE;
				else if (!strcmp("info", (yyvsp[0].string)))
					(yyval.val) = NFT_LOGLEVEL_INFO;
				else if (!strcmp("debug", (yyvsp[0].string)))
					(yyval.val) = NFT_LOGLEVEL_DEBUG;
				else if (!strcmp("audit", (yyvsp[0].string)))
					(yyval.val) = NFT_LOGLEVEL_AUDIT;
				else {
					erec_queue(error(&(yylsp[0]), "invalid log level"),
						   state->msgs);
					free_const((yyvsp[0].string));
					YYERROR;
				}
				free_const((yyvsp[0].string));
			}
#line 11987 "src/parser_bison.c"
    break;

  case 558: /* log_flags: "tcp" log_flags_tcp close_scope_tcp  */
#line 3457 "src/parser_bison.y"
                        {
				(yyval.val) = (yyvsp[-1].val);
			}
#line 11995 "src/parser_bison.c"
    break;

  case 559: /* log_flags: "ip" "options" close_scope_ip  */
#line 3461 "src/parser_bison.y"
                        {
				(yyval.val) = NF_LOG_IPOPT;
			}
#line 12003 "src/parser_bison.c"
    break;

  case 560: /* log_flags: "skuid"  */
#line 3465 "src/parser_bison.y"
                        {
				(yyval.val) = NF_LOG_UID;
			}
#line 12011 "src/parser_bison.c"
    break;

  case 561: /* log_flags: "ether" close_scope_eth  */
#line 3469 "src/parser_bison.y"
                        {
				(yyval.val) = NF_LOG_MACDECODE;
			}
#line 12019 "src/parser_bison.c"
    break;

  case 562: /* log_flags: "all"  */
#line 3473 "src/parser_bison.y"
                        {
				(yyval.val) = NF_LOG_MASK;
			}
#line 12027 "src/parser_bison.c"
    break;

  case 563: /* log_flags_tcp: log_flags_tcp "comma" log_flag_tcp  */
#line 3479 "src/parser_bison.y"
                        {
				(yyval.val) = (yyvsp[-2].val) | (yyvsp[0].val);
			}
#line 12035 "src/parser_bison.c"
    break;

  case 565: /* log_flag_tcp: "seq"  */
#line 3486 "src/parser_bison.y"
                        {
				(yyval.val) = NF_LOG_TCPSEQ;
			}
#line 12043 "src/parser_bison.c"
    break;

  case 566: /* log_flag_tcp: "options"  */
#line 3490 "src/parser_bison.y"
                        {
				(yyval.val) = NF_LOG_TCPOPT;
			}
#line 12051 "src/parser_bison.c"
    break;

  case 567: /* limit_stmt_alloc: "limit" "rate"  */
#line 3496 "src/parser_bison.y"
                        {
				(yyval.stmt) = limit_stmt_alloc(&(yyloc));
			}
#line 12059 "src/parser_bison.c"
    break;

  case 569: /* limit_args: limit_mode limit_rate_pkts limit_burst_pkts  */
#line 3505 "src/parser_bison.y"
                        {
				struct limit_stmt *limit;

				assert((yyvsp[-3].stmt)->type == STMT_LIMIT);

				if ((yyvsp[0].val) == 0) {
					erec_queue(error(&(yylsp[0]), "packet limit burst must be > 0"),
						   state->msgs);
					YYERROR;
				}
				limit = &(yyvsp[-3].stmt)->limit;
				limit->rate = (yyvsp[-1].limit_rate).rate;
				limit->unit = (yyvsp[-1].limit_rate).unit;
				limit->burst = (yyvsp[0].val);
				limit->type = NFT_LIMIT_PKTS;
				limit->flags = (yyvsp[-2].val);
			}
#line 12081 "src/parser_bison.c"
    break;

  case 570: /* limit_args: limit_mode limit_rate_bytes limit_burst_bytes  */
#line 3523 "src/parser_bison.y"
                        {
				struct limit_stmt *limit;

				assert((yyvsp[-3].stmt)->type == STMT_LIMIT);

				limit = &(yyvsp[-3].stmt)->limit;
				limit->rate = (yyvsp[-1].limit_rate).rate;
				limit->unit = (yyvsp[-1].limit_rate).unit;
				limit->burst = (yyvsp[0].val);
				limit->type = NFT_LIMIT_PKT_BYTES;
				limit->flags = (yyvsp[-2].val);
			}
#line 12098 "src/parser_bison.c"
    break;

  case 571: /* quota_mode: "over"  */
#line 3537 "src/parser_bison.y"
                                                { (yyval.val) = NFT_QUOTA_F_INV; }
#line 12104 "src/parser_bison.c"
    break;

  case 572: /* quota_mode: "until"  */
#line 3538 "src/parser_bison.y"
                                                { (yyval.val) = 0; }
#line 12110 "src/parser_bison.c"
    break;

  case 573: /* quota_mode: %empty  */
#line 3539 "src/parser_bison.y"
                                                { (yyval.val) = 0; }
#line 12116 "src/parser_bison.c"
    break;

  case 574: /* quota_unit: "bytes"  */
#line 3542 "src/parser_bison.y"
                                                { (yyval.string) = xstrdup("bytes"); }
#line 12122 "src/parser_bison.c"
    break;

  case 575: /* quota_unit: "string"  */
#line 3543 "src/parser_bison.y"
                                                { (yyval.string) = (yyvsp[0].string); }
#line 12128 "src/parser_bison.c"
    break;

  case 576: /* quota_used: %empty  */
#line 3546 "src/parser_bison.y"
                                                { (yyval.val) = 0; }
#line 12134 "src/parser_bison.c"
    break;

  case 577: /* quota_used: "used" "number" quota_unit  */
#line 3548 "src/parser_bison.y"
                        {
				struct error_record *erec;
				uint64_t rate;

				erec = data_unit_parse(&(yyloc), (yyvsp[0].string), &rate);
				free_const((yyvsp[0].string));
				if (erec != NULL) {
					erec_queue(erec, state->msgs);
					YYERROR;
				}
				(yyval.val) = (yyvsp[-1].val) * rate;
			}
#line 12151 "src/parser_bison.c"
    break;

  case 578: /* quota_stmt_alloc: "quota"  */
#line 3563 "src/parser_bison.y"
                        {
				(yyval.stmt) = quota_stmt_alloc(&(yyloc));
			}
#line 12159 "src/parser_bison.c"
    break;

  case 580: /* quota_args: quota_mode "number" quota_unit quota_used  */
#line 3572 "src/parser_bison.y"
                        {
				struct error_record *erec;
				struct quota_stmt *quota;
				uint64_t rate;

				assert((yyvsp[-4].stmt)->type == STMT_QUOTA);

				erec = data_unit_parse(&(yyloc), (yyvsp[-1].string), &rate);
				free_const((yyvsp[-1].string));
				if (erec != NULL) {
					erec_queue(erec, state->msgs);
					YYERROR;
				}
				quota = &(yyvsp[-4].stmt)->quota;
				quota->bytes = (yyvsp[-2].val) * rate;
				quota->used = (yyvsp[0].val);
				quota->flags = (yyvsp[-3].val);
			}
#line 12182 "src/parser_bison.c"
    break;

  case 581: /* limit_mode: "over"  */
#line 3592 "src/parser_bison.y"
                                                                { (yyval.val) = NFT_LIMIT_F_INV; }
#line 12188 "src/parser_bison.c"
    break;

  case 582: /* limit_mode: "until"  */
#line 3593 "src/parser_bison.y"
                                                                { (yyval.val) = 0; }
#line 12194 "src/parser_bison.c"
    break;

  case 583: /* limit_mode: %empty  */
#line 3594 "src/parser_bison.y"
                                                                { (yyval.val) = 0; }
#line 12200 "src/parser_bison.c"
    break;

  case 584: /* limit_burst_pkts: %empty  */
#line 3597 "src/parser_bison.y"
                                                                { (yyval.val) = 5; }
#line 12206 "src/parser_bison.c"
    break;

  case 585: /* limit_burst_pkts: "burst" "number" "packets"  */
#line 3598 "src/parser_bison.y"
                                                                { (yyval.val) = (yyvsp[-1].val); }
#line 12212 "src/parser_bison.c"
    break;

  case 586: /* limit_rate_pkts: "number" "/" time_unit  */
#line 3602 "src/parser_bison.y"
                        {
				(yyval.limit_rate).rate = (yyvsp[-2].val);
				(yyval.limit_rate).unit = (yyvsp[0].val);
			}
#line 12221 "src/parser_bison.c"
    break;

  case 587: /* limit_burst_bytes: %empty  */
#line 3608 "src/parser_bison.y"
                                                                { (yyval.val) = 0; }
#line 12227 "src/parser_bison.c"
    break;

  case 588: /* limit_burst_bytes: "burst" limit_bytes  */
#line 3609 "src/parser_bison.y"
                                                                { (yyval.val) = (yyvsp[0].val); }
#line 12233 "src/parser_bison.c"
    break;

  case 589: /* limit_rate_bytes: "number" "string"  */
#line 3613 "src/parser_bison.y"
                        {
				struct error_record *erec;
				uint64_t rate, unit;

				erec = rate_parse(&(yyloc), (yyvsp[0].string), &rate, &unit);
				free_const((yyvsp[0].string));
				if (erec != NULL) {
					erec_queue(erec, state->msgs);
					YYERROR;
				}
				(yyval.limit_rate).rate = rate * (yyvsp[-1].val);
				(yyval.limit_rate).unit = unit;
			}
#line 12251 "src/parser_bison.c"
    break;

  case 590: /* limit_rate_bytes: limit_bytes "/" time_unit  */
#line 3627 "src/parser_bison.y"
                        {
				(yyval.limit_rate).rate = (yyvsp[-2].val);
				(yyval.limit_rate).unit = (yyvsp[0].val);
			}
#line 12260 "src/parser_bison.c"
    break;

  case 591: /* limit_bytes: "number" "bytes"  */
#line 3633 "src/parser_bison.y"
                                                        { (yyval.val) = (yyvsp[-1].val); }
#line 12266 "src/parser_bison.c"
    break;

  case 592: /* limit_bytes: "number" "string"  */
#line 3635 "src/parser_bison.y"
                        {
				struct error_record *erec;
				uint64_t rate;

				erec = data_unit_parse(&(yyloc), (yyvsp[0].string), &rate);
				free_const((yyvsp[0].string));
				if (erec != NULL) {
					erec_queue(erec, state->msgs);
					YYERROR;
				}
				(yyval.val) = (yyvsp[-1].val) * rate;
			}
#line 12283 "src/parser_bison.c"
    break;

  case 593: /* time_unit: "second"  */
#line 3649 "src/parser_bison.y"
                                                { (yyval.val) = 1ULL; }
#line 12289 "src/parser_bison.c"
    break;

  case 594: /* time_unit: "minute"  */
#line 3650 "src/parser_bison.y"
                                                { (yyval.val) = 1ULL * 60; }
#line 12295 "src/parser_bison.c"
    break;

  case 595: /* time_unit: "hour"  */
#line 3651 "src/parser_bison.y"
                                                { (yyval.val) = 1ULL * 60 * 60; }
#line 12301 "src/parser_bison.c"
    break;

  case 596: /* time_unit: "day"  */
#line 3652 "src/parser_bison.y"
                                                { (yyval.val) = 1ULL * 60 * 60 * 24; }
#line 12307 "src/parser_bison.c"
    break;

  case 597: /* time_unit: "week"  */
#line 3653 "src/parser_bison.y"
                                                { (yyval.val) = 1ULL * 60 * 60 * 24 * 7; }
#line 12313 "src/parser_bison.c"
    break;

  case 599: /* reject_stmt_alloc: "reject"  */
#line 3660 "src/parser_bison.y"
                        {
				(yyval.stmt) = reject_stmt_alloc(&(yyloc));
			}
#line 12321 "src/parser_bison.c"
    break;

  case 600: /* reject_with_expr: "string"  */
#line 3666 "src/parser_bison.y"
                        {
				(yyval.expr) = symbol_expr_alloc(&(yyloc), SYMBOL_VALUE,
						       current_scope(state), (yyvsp[0].string));
				free_const((yyvsp[0].string));
			}
#line 12331 "src/parser_bison.c"
    break;

  case 601: /* reject_with_expr: integer_expr  */
#line 3671 "src/parser_bison.y"
                                                { (yyval.expr) = (yyvsp[0].expr); }
#line 12337 "src/parser_bison.c"
    break;

  case 602: /* reject_opts: %empty  */
#line 3675 "src/parser_bison.y"
                        {
				(yyvsp[0].stmt)->reject.type = -1;
				(yyvsp[0].stmt)->reject.icmp_code = -1;
			}
#line 12346 "src/parser_bison.c"
    break;

  case 603: /* reject_opts: "with" "icmp" "type" reject_with_expr close_scope_type close_scope_icmp  */
#line 3680 "src/parser_bison.y"
                        {
				(yyvsp[-6].stmt)->reject.family = NFPROTO_IPV4;
				(yyvsp[-6].stmt)->reject.type = NFT_REJECT_ICMP_UNREACH;
				(yyvsp[-6].stmt)->reject.expr = (yyvsp[-2].expr);
				datatype_set((yyvsp[-6].stmt)->reject.expr, &reject_icmp_code_type);
			}
#line 12357 "src/parser_bison.c"
    break;

  case 604: /* reject_opts: "with" "icmp" reject_with_expr  */
#line 3687 "src/parser_bison.y"
                        {
				(yyvsp[-3].stmt)->reject.family = NFPROTO_IPV4;
				(yyvsp[-3].stmt)->reject.type = NFT_REJECT_ICMP_UNREACH;
				(yyvsp[-3].stmt)->reject.expr = (yyvsp[0].expr);
				datatype_set((yyvsp[-3].stmt)->reject.expr, &reject_icmp_code_type);
			}
#line 12368 "src/parser_bison.c"
    break;

  case 605: /* reject_opts: "with" "icmpv6" "type" reject_with_expr close_scope_type close_scope_icmp  */
#line 3694 "src/parser_bison.y"
                        {
				(yyvsp[-6].stmt)->reject.family = NFPROTO_IPV6;
				(yyvsp[-6].stmt)->reject.type = NFT_REJECT_ICMP_UNREACH;
				(yyvsp[-6].stmt)->reject.expr = (yyvsp[-2].expr);
				datatype_set((yyvsp[-6].stmt)->reject.expr, &reject_icmpv6_code_type);
			}
#line 12379 "src/parser_bison.c"
    break;

  case 606: /* reject_opts: "with" "icmpv6" reject_with_expr  */
#line 3701 "src/parser_bison.y"
                        {
				(yyvsp[-3].stmt)->reject.family = NFPROTO_IPV6;
				(yyvsp[-3].stmt)->reject.type = NFT_REJECT_ICMP_UNREACH;
				(yyvsp[-3].stmt)->reject.expr = (yyvsp[0].expr);
				datatype_set((yyvsp[-3].stmt)->reject.expr, &reject_icmpv6_code_type);
			}
#line 12390 "src/parser_bison.c"
    break;

  case 607: /* reject_opts: "with" "icmpx" "type" reject_with_expr close_scope_type  */
#line 3708 "src/parser_bison.y"
                        {
				(yyvsp[-5].stmt)->reject.type = NFT_REJECT_ICMPX_UNREACH;
				(yyvsp[-5].stmt)->reject.expr = (yyvsp[-1].expr);
				datatype_set((yyvsp[-5].stmt)->reject.expr, &reject_icmpx_code_type);
			}
#line 12400 "src/parser_bison.c"
    break;

  case 608: /* reject_opts: "with" "icmpx" reject_with_expr  */
#line 3714 "src/parser_bison.y"
                        {
				(yyvsp[-3].stmt)->reject.type = NFT_REJECT_ICMPX_UNREACH;
				(yyvsp[-3].stmt)->reject.expr = (yyvsp[0].expr);
				datatype_set((yyvsp[-3].stmt)->reject.expr, &reject_icmpx_code_type);
			}
#line 12410 "src/parser_bison.c"
    break;

  case 609: /* reject_opts: "with" "tcp" close_scope_tcp "reset" close_scope_reset  */
#line 3720 "src/parser_bison.y"
                        {
				(yyvsp[-5].stmt)->reject.type = NFT_REJECT_TCP_RST;
			}
#line 12418 "src/parser_bison.c"
    break;

  case 611: /* nat_stmt_alloc: "snat"  */
#line 3728 "src/parser_bison.y"
                                        { (yyval.stmt) = nat_stmt_alloc(&(yyloc), __NFT_NAT_SNAT); }
#line 12424 "src/parser_bison.c"
    break;

  case 612: /* nat_stmt_alloc: "dnat"  */
#line 3729 "src/parser_bison.y"
                                        { (yyval.stmt) = nat_stmt_alloc(&(yyloc), __NFT_NAT_DNAT); }
#line 12430 "src/parser_bison.c"
    break;

  case 613: /* tproxy_stmt: "tproxy" "to" stmt_expr  */
#line 3733 "src/parser_bison.y"
                        {
				(yyval.stmt) = tproxy_stmt_alloc(&(yyloc));
				(yyval.stmt)->tproxy.family = NFPROTO_UNSPEC;
				(yyval.stmt)->tproxy.addr = (yyvsp[0].expr);
			}
#line 12440 "src/parser_bison.c"
    break;

  case 614: /* tproxy_stmt: "tproxy" nf_key_proto "to" stmt_expr  */
#line 3739 "src/parser_bison.y"
                        {
				(yyval.stmt) = tproxy_stmt_alloc(&(yyloc));
				(yyval.stmt)->tproxy.family = (yyvsp[-2].val);
				(yyval.stmt)->tproxy.addr = (yyvsp[0].expr);
			}
#line 12450 "src/parser_bison.c"
    break;

  case 615: /* tproxy_stmt: "tproxy" "to" "colon" stmt_expr  */
#line 3745 "src/parser_bison.y"
                        {
				(yyval.stmt) = tproxy_stmt_alloc(&(yyloc));
				(yyval.stmt)->tproxy.family = NFPROTO_UNSPEC;
				(yyval.stmt)->tproxy.port = (yyvsp[0].expr);
			}
#line 12460 "src/parser_bison.c"
    break;

  case 616: /* tproxy_stmt: "tproxy" "to" stmt_expr "colon" stmt_expr  */
#line 3751 "src/parser_bison.y"
                        {
				(yyval.stmt) = tproxy_stmt_alloc(&(yyloc));
				(yyval.stmt)->tproxy.family = NFPROTO_UNSPEC;
				(yyval.stmt)->tproxy.addr = (yyvsp[-2].expr);
				(yyval.stmt)->tproxy.port = (yyvsp[0].expr);
			}
#line 12471 "src/parser_bison.c"
    break;

  case 617: /* tproxy_stmt: "tproxy" nf_key_proto "to" stmt_expr "colon" stmt_expr  */
#line 3758 "src/parser_bison.y"
                        {
				(yyval.stmt) = tproxy_stmt_alloc(&(yyloc));
				(yyval.stmt)->tproxy.family = (yyvsp[-4].val);
				(yyval.stmt)->tproxy.addr = (yyvsp[-2].expr);
				(yyval.stmt)->tproxy.port = (yyvsp[0].expr);
			}
#line 12482 "src/parser_bison.c"
    break;

  case 618: /* tproxy_stmt: "tproxy" nf_key_proto "to" "colon" stmt_expr  */
#line 3765 "src/parser_bison.y"
                        {
				(yyval.stmt) = tproxy_stmt_alloc(&(yyloc));
				(yyval.stmt)->tproxy.family = (yyvsp[-3].val);
				(yyval.stmt)->tproxy.port = (yyvsp[0].expr);
			}
#line 12492 "src/parser_bison.c"
    break;

  case 621: /* synproxy_stmt_alloc: "synproxy"  */
#line 3777 "src/parser_bison.y"
                        {
				(yyval.stmt) = synproxy_stmt_alloc(&(yyloc));
			}
#line 12500 "src/parser_bison.c"
    break;

  case 622: /* synproxy_args: synproxy_arg  */
#line 3783 "src/parser_bison.y"
                        {
				(yyval.stmt)	= (yyvsp[-1].stmt);
			}
#line 12508 "src/parser_bison.c"
    break;

  case 624: /* synproxy_arg: "mss" "number"  */
#line 3790 "src/parser_bison.y"
                        {
				(yyvsp[-2].stmt)->synproxy.mss = (yyvsp[0].val);
				(yyvsp[-2].stmt)->synproxy.flags |= NF_SYNPROXY_OPT_MSS;
			}
#line 12517 "src/parser_bison.c"
    break;

  case 625: /* synproxy_arg: "wscale" "number"  */
#line 3795 "src/parser_bison.y"
                        {
				(yyvsp[-2].stmt)->synproxy.wscale = (yyvsp[0].val);
				(yyvsp[-2].stmt)->synproxy.flags |= NF_SYNPROXY_OPT_WSCALE;
			}
#line 12526 "src/parser_bison.c"
    break;

  case 626: /* synproxy_arg: "timestamp"  */
#line 3800 "src/parser_bison.y"
                        {
				(yyvsp[-1].stmt)->synproxy.flags |= NF_SYNPROXY_OPT_TIMESTAMP;
			}
#line 12534 "src/parser_bison.c"
    break;

  case 627: /* synproxy_arg: "sack-permitted"  */
#line 3804 "src/parser_bison.y"
                        {
				(yyvsp[-1].stmt)->synproxy.flags |= NF_SYNPROXY_OPT_SACK_PERM;
			}
#line 12542 "src/parser_bison.c"
    break;

  case 628: /* synproxy_config: "mss" "number" "wscale" "number" synproxy_ts synproxy_sack  */
#line 3810 "src/parser_bison.y"
                        {
				struct synproxy *synproxy;
				uint32_t flags = 0;

				synproxy = &(yyvsp[-6].obj)->synproxy;
				synproxy->mss = (yyvsp[-4].val);
				flags |= NF_SYNPROXY_OPT_MSS;
				synproxy->wscale = (yyvsp[-2].val);
				flags |= NF_SYNPROXY_OPT_WSCALE;
				if ((yyvsp[-1].val))
					flags |= (yyvsp[-1].val);
				if ((yyvsp[0].val))
					flags |= (yyvsp[0].val);
				synproxy->flags = flags;
			}
#line 12562 "src/parser_bison.c"
    break;

  case 629: /* synproxy_config: "mss" "number" stmt_separator "wscale" "number" stmt_separator synproxy_ts synproxy_sack  */
#line 3826 "src/parser_bison.y"
                        {
				struct synproxy *synproxy;
				uint32_t flags = 0;

				synproxy = &(yyvsp[-8].obj)->synproxy;
				synproxy->mss = (yyvsp[-6].val);
				flags |= NF_SYNPROXY_OPT_MSS;
				synproxy->wscale = (yyvsp[-3].val);
				flags |= NF_SYNPROXY_OPT_WSCALE;
				if ((yyvsp[-1].val))
					flags |= (yyvsp[-1].val);
				if ((yyvsp[0].val))
					flags |= (yyvsp[0].val);
				synproxy->flags = flags;
			}
#line 12582 "src/parser_bison.c"
    break;

  case 630: /* synproxy_obj: %empty  */
#line 3844 "src/parser_bison.y"
                        {
				(yyval.obj) = obj_alloc(&(yyloc));
				(yyval.obj)->type = NFT_OBJECT_SYNPROXY;
			}
#line 12591 "src/parser_bison.c"
    break;

  case 631: /* synproxy_ts: %empty  */
#line 3850 "src/parser_bison.y"
                                                { (yyval.val) = 0; }
#line 12597 "src/parser_bison.c"
    break;

  case 632: /* synproxy_ts: "timestamp"  */
#line 3852 "src/parser_bison.y"
                        {
				(yyval.val) = NF_SYNPROXY_OPT_TIMESTAMP;
			}
#line 12605 "src/parser_bison.c"
    break;

  case 633: /* synproxy_sack: %empty  */
#line 3857 "src/parser_bison.y"
                                                { (yyval.val) = 0; }
#line 12611 "src/parser_bison.c"
    break;

  case 634: /* synproxy_sack: "sack-permitted"  */
#line 3859 "src/parser_bison.y"
                        {
				(yyval.val) = NF_SYNPROXY_OPT_SACK_PERM;
			}
#line 12619 "src/parser_bison.c"
    break;

  case 635: /* primary_stmt_expr: symbol_expr  */
#line 3864 "src/parser_bison.y"
                                                                { (yyval.expr) = (yyvsp[0].expr); }
#line 12625 "src/parser_bison.c"
    break;

  case 636: /* primary_stmt_expr: integer_expr  */
#line 3865 "src/parser_bison.y"
                                                                { (yyval.expr) = (yyvsp[0].expr); }
#line 12631 "src/parser_bison.c"
    break;

  case 637: /* primary_stmt_expr: boolean_expr  */
#line 3866 "src/parser_bison.y"
                                                                { (yyval.expr) = (yyvsp[0].expr); }
#line 12637 "src/parser_bison.c"
    break;

  case 638: /* primary_stmt_expr: meta_expr  */
#line 3867 "src/parser_bison.y"
                                                                { (yyval.expr) = (yyvsp[0].expr); }
#line 12643 "src/parser_bison.c"
    break;

  case 639: /* primary_stmt_expr: rt_expr  */
#line 3868 "src/parser_bison.y"
                                                                { (yyval.expr) = (yyvsp[0].expr); }
#line 12649 "src/parser_bison.c"
    break;

  case 640: /* primary_stmt_expr: ct_expr  */
#line 3869 "src/parser_bison.y"
                                                                { (yyval.expr) = (yyvsp[0].expr); }
#line 12655 "src/parser_bison.c"
    break;

  case 641: /* primary_stmt_expr: numgen_expr  */
#line 3870 "src/parser_bison.y"
                                                                { (yyval.expr) = (yyvsp[0].expr); }
#line 12661 "src/parser_bison.c"
    break;

  case 642: /* primary_stmt_expr: hash_expr  */
#line 3871 "src/parser_bison.y"
                                                                { (yyval.expr) = (yyvsp[0].expr); }
#line 12667 "src/parser_bison.c"
    break;

  case 643: /* primary_stmt_expr: payload_expr  */
#line 3872 "src/parser_bison.y"
                                                                { (yyval.expr) = (yyvsp[0].expr); }
#line 12673 "src/parser_bison.c"
    break;

  case 644: /* primary_stmt_expr: keyword_expr  */
#line 3873 "src/parser_bison.y"
                                                                { (yyval.expr) = (yyvsp[0].expr); }
#line 12679 "src/parser_bison.c"
    break;

  case 645: /* primary_stmt_expr: socket_expr  */
#line 3874 "src/parser_bison.y"
                                                                { (yyval.expr) = (yyvsp[0].expr); }
#line 12685 "src/parser_bison.c"
    break;

  case 646: /* primary_stmt_expr: osf_expr  */
#line 3875 "src/parser_bison.y"
                                                                { (yyval.expr) = (yyvsp[0].expr); }
#line 12691 "src/parser_bison.c"
    break;

  case 647: /* primary_stmt_expr: '(' basic_stmt_expr ')'  */
#line 3876 "src/parser_bison.y"
                                                                { (yyval.expr) = (yyvsp[-1].expr); }
#line 12697 "src/parser_bison.c"
    break;

  case 649: /* shift_stmt_expr: shift_stmt_expr "<<" primary_stmt_expr  */
#line 3881 "src/parser_bison.y"
                        {
				(yyval.expr) = binop_expr_alloc(&(yyloc), OP_LSHIFT, (yyvsp[-2].expr), (yyvsp[0].expr));
			}
#line 12705 "src/parser_bison.c"
    break;

  case 650: /* shift_stmt_expr: shift_stmt_expr ">>" primary_stmt_expr  */
#line 3885 "src/parser_bison.y"
                        {
				(yyval.expr) = binop_expr_alloc(&(yyloc), OP_RSHIFT, (yyvsp[-2].expr), (yyvsp[0].expr));
			}
#line 12713 "src/parser_bison.c"
    break;

  case 652: /* and_stmt_expr: and_stmt_expr "&" shift_stmt_expr  */
#line 3892 "src/parser_bison.y"
                        {
				(yyval.expr) = binop_expr_alloc(&(yyloc), OP_AND, (yyvsp[-2].expr), (yyvsp[0].expr));
			}
#line 12721 "src/parser_bison.c"
    break;

  case 654: /* exclusive_or_stmt_expr: exclusive_or_stmt_expr "^" and_stmt_expr  */
#line 3899 "src/parser_bison.y"
                        {
				(yyval.expr) = binop_expr_alloc(&(yyloc), OP_XOR, (yyvsp[-2].expr), (yyvsp[0].expr));
			}
#line 12729 "src/parser_bison.c"
    break;

  case 656: /* inclusive_or_stmt_expr: inclusive_or_stmt_expr '|' exclusive_or_stmt_expr  */
#line 3906 "src/parser_bison.y"
                        {
				(yyval.expr) = binop_expr_alloc(&(yyloc), OP_OR, (yyvsp[-2].expr), (yyvsp[0].expr));
			}
#line 12737 "src/parser_bison.c"
    break;

  case 659: /* concat_stmt_expr: concat_stmt_expr "." primary_stmt_expr  */
#line 3916 "src/parser_bison.y"
                        {
				struct location rhs[] = {
					[1]	= (yylsp[-1]),
					[2]	= (yylsp[0]),
				};

				(yyval.expr) = handle_concat_expr(&(yyloc), (yyval.expr), (yyvsp[-2].expr), (yyvsp[0].expr), rhs);
			}
#line 12750 "src/parser_bison.c"
    break;

  case 662: /* map_stmt_expr: concat_stmt_expr "map" map_stmt_expr_set  */
#line 3931 "src/parser_bison.y"
                        {
				(yyval.expr) = map_expr_alloc(&(yyloc), (yyvsp[-2].expr), (yyvsp[0].expr));
			}
#line 12758 "src/parser_bison.c"
    break;

  case 663: /* map_stmt_expr: concat_stmt_expr  */
#line 3934 "src/parser_bison.y"
                                                        { (yyval.expr) = (yyvsp[0].expr); }
#line 12764 "src/parser_bison.c"
    break;

  case 664: /* prefix_stmt_expr: basic_stmt_expr "/" "number"  */
#line 3938 "src/parser_bison.y"
                        {
				(yyval.expr) = prefix_expr_alloc(&(yyloc), (yyvsp[-2].expr), (yyvsp[0].val));
			}
#line 12772 "src/parser_bison.c"
    break;

  case 665: /* range_stmt_expr: basic_stmt_expr "-" basic_stmt_expr  */
#line 3944 "src/parser_bison.y"
                        {
				(yyval.expr) = range_expr_alloc(&(yyloc), (yyvsp[-2].expr), (yyvsp[0].expr));
			}
#line 12780 "src/parser_bison.c"
    break;

  case 671: /* nat_stmt_args: stmt_expr  */
#line 3959 "src/parser_bison.y"
                        {
				(yyvsp[-1].stmt)->nat.addr = (yyvsp[0].expr);
			}
#line 12788 "src/parser_bison.c"
    break;

  case 672: /* nat_stmt_args: "to" stmt_expr  */
#line 3963 "src/parser_bison.y"
                        {
				(yyvsp[-2].stmt)->nat.addr = (yyvsp[0].expr);
			}
#line 12796 "src/parser_bison.c"
    break;

  case 673: /* nat_stmt_args: nf_key_proto "to" stmt_expr  */
#line 3967 "src/parser_bison.y"
                        {
				(yyvsp[-3].stmt)->nat.family = (yyvsp[-2].val);
				(yyvsp[-3].stmt)->nat.addr = (yyvsp[0].expr);
			}
#line 12805 "src/parser_bison.c"
    break;

  case 674: /* nat_stmt_args: stmt_expr "colon" stmt_expr  */
#line 3972 "src/parser_bison.y"
                        {
				(yyvsp[-3].stmt)->nat.addr = (yyvsp[-2].expr);
				(yyvsp[-3].stmt)->nat.proto = (yyvsp[0].expr);
			}
#line 12814 "src/parser_bison.c"
    break;

  case 675: /* nat_stmt_args: "to" stmt_expr "colon" stmt_expr  */
#line 3977 "src/parser_bison.y"
                        {
				(yyvsp[-4].stmt)->nat.addr = (yyvsp[-2].expr);
				(yyvsp[-4].stmt)->nat.proto = (yyvsp[0].expr);
			}
#line 12823 "src/parser_bison.c"
    break;

  case 676: /* nat_stmt_args: nf_key_proto "to" stmt_expr "colon" stmt_expr  */
#line 3982 "src/parser_bison.y"
                        {
				(yyvsp[-5].stmt)->nat.family = (yyvsp[-4].val);
				(yyvsp[-5].stmt)->nat.addr = (yyvsp[-2].expr);
				(yyvsp[-5].stmt)->nat.proto = (yyvsp[0].expr);
			}
#line 12833 "src/parser_bison.c"
    break;

  case 677: /* nat_stmt_args: "colon" stmt_expr  */
#line 3988 "src/parser_bison.y"
                        {
				(yyvsp[-2].stmt)->nat.proto = (yyvsp[0].expr);
			}
#line 12841 "src/parser_bison.c"
    break;

  case 678: /* nat_stmt_args: "to" "colon" stmt_expr  */
#line 3992 "src/parser_bison.y"
                        {
				(yyvsp[-3].stmt)->nat.proto = (yyvsp[0].expr);
			}
#line 12849 "src/parser_bison.c"
    break;

  case 679: /* nat_stmt_args: nat_stmt_args nf_nat_flags  */
#line 3996 "src/parser_bison.y"
                        {
				(yyvsp[-2].stmt)->nat.flags = (yyvsp[0].val);
			}
#line 12857 "src/parser_bison.c"
    break;

  case 680: /* nat_stmt_args: nf_key_proto "addr" "." "port" "to" stmt_expr  */
#line 4000 "src/parser_bison.y"
                        {
				(yyvsp[-6].stmt)->nat.family = (yyvsp[-5].val);
				(yyvsp[-6].stmt)->nat.addr = (yyvsp[0].expr);
				(yyvsp[-6].stmt)->nat.type_flags = STMT_NAT_F_CONCAT;
			}
#line 12867 "src/parser_bison.c"
    break;

  case 681: /* nat_stmt_args: nf_key_proto "interval" "to" stmt_expr  */
#line 4006 "src/parser_bison.y"
                        {
				(yyvsp[-4].stmt)->nat.family = (yyvsp[-3].val);
				(yyvsp[-4].stmt)->nat.addr = (yyvsp[0].expr);
			}
#line 12876 "src/parser_bison.c"
    break;

  case 682: /* nat_stmt_args: "interval" "to" stmt_expr  */
#line 4011 "src/parser_bison.y"
                        {
				(yyvsp[-3].stmt)->nat.addr = (yyvsp[0].expr);
			}
#line 12884 "src/parser_bison.c"
    break;

  case 683: /* nat_stmt_args: nf_key_proto "prefix" "to" stmt_expr  */
#line 4015 "src/parser_bison.y"
                        {
				(yyvsp[-4].stmt)->nat.family = (yyvsp[-3].val);
				(yyvsp[-4].stmt)->nat.addr = (yyvsp[0].expr);
				(yyvsp[-4].stmt)->nat.type_flags =
						STMT_NAT_F_PREFIX;
				(yyvsp[-4].stmt)->nat.flags |= NF_NAT_RANGE_NETMAP;
			}
#line 12896 "src/parser_bison.c"
    break;

  case 684: /* nat_stmt_args: "prefix" "to" stmt_expr  */
#line 4023 "src/parser_bison.y"
                        {
				(yyvsp[-3].stmt)->nat.addr = (yyvsp[0].expr);
				(yyvsp[-3].stmt)->nat.type_flags =
						STMT_NAT_F_PREFIX;
				(yyvsp[-3].stmt)->nat.flags |= NF_NAT_RANGE_NETMAP;
			}
#line 12907 "src/parser_bison.c"
    break;

  case 687: /* masq_stmt_alloc: "masquerade"  */
#line 4035 "src/parser_bison.y"
                                                { (yyval.stmt) = nat_stmt_alloc(&(yyloc), NFT_NAT_MASQ); }
#line 12913 "src/parser_bison.c"
    break;

  case 688: /* masq_stmt_args: "to" "colon" stmt_expr  */
#line 4039 "src/parser_bison.y"
                        {
				(yyvsp[-3].stmt)->nat.proto = (yyvsp[0].expr);
			}
#line 12921 "src/parser_bison.c"
    break;

  case 689: /* masq_stmt_args: "to" "colon" stmt_expr nf_nat_flags  */
#line 4043 "src/parser_bison.y"
                        {
				(yyvsp[-4].stmt)->nat.proto = (yyvsp[-1].expr);
				(yyvsp[-4].stmt)->nat.flags = (yyvsp[0].val);
			}
#line 12930 "src/parser_bison.c"
    break;

  case 690: /* masq_stmt_args: nf_nat_flags  */
#line 4048 "src/parser_bison.y"
                        {
				(yyvsp[-1].stmt)->nat.flags = (yyvsp[0].val);
			}
#line 12938 "src/parser_bison.c"
    break;

  case 693: /* redir_stmt_alloc: "redirect"  */
#line 4057 "src/parser_bison.y"
                                                { (yyval.stmt) = nat_stmt_alloc(&(yyloc), NFT_NAT_REDIR); }
#line 12944 "src/parser_bison.c"
    break;

  case 694: /* redir_stmt_arg: "to" stmt_expr  */
#line 4061 "src/parser_bison.y"
                        {
				(yyvsp[-2].stmt)->nat.proto = (yyvsp[0].expr);
			}
#line 12952 "src/parser_bison.c"
    break;

  case 695: /* redir_stmt_arg: "to" "colon" stmt_expr  */
#line 4065 "src/parser_bison.y"
                        {
				(yyvsp[-3].stmt)->nat.proto = (yyvsp[0].expr);
			}
#line 12960 "src/parser_bison.c"
    break;

  case 696: /* redir_stmt_arg: nf_nat_flags  */
#line 4069 "src/parser_bison.y"
                        {
				(yyvsp[-1].stmt)->nat.flags = (yyvsp[0].val);
			}
#line 12968 "src/parser_bison.c"
    break;

  case 697: /* redir_stmt_arg: "to" stmt_expr nf_nat_flags  */
#line 4073 "src/parser_bison.y"
                        {
				(yyvsp[-3].stmt)->nat.proto = (yyvsp[-1].expr);
				(yyvsp[-3].stmt)->nat.flags = (yyvsp[0].val);
			}
#line 12977 "src/parser_bison.c"
    break;

  case 698: /* redir_stmt_arg: "to" "colon" stmt_expr nf_nat_flags  */
#line 4078 "src/parser_bison.y"
                        {
				(yyvsp[-4].stmt)->nat.proto = (yyvsp[-1].expr);
				(yyvsp[-4].stmt)->nat.flags = (yyvsp[0].val);
			}
#line 12986 "src/parser_bison.c"
    break;

  case 699: /* dup_stmt: "dup" "to" stmt_expr  */
#line 4085 "src/parser_bison.y"
                        {
				(yyval.stmt) = dup_stmt_alloc(&(yyloc));
				(yyval.stmt)->dup.to = (yyvsp[0].expr);
			}
#line 12995 "src/parser_bison.c"
    break;

  case 700: /* dup_stmt: "dup" "to" stmt_expr "device" stmt_expr  */
#line 4090 "src/parser_bison.y"
                        {
				(yyval.stmt) = dup_stmt_alloc(&(yyloc));
				(yyval.stmt)->dup.to = (yyvsp[-2].expr);
				(yyval.stmt)->dup.dev = (yyvsp[0].expr);
			}
#line 13005 "src/parser_bison.c"
    break;

  case 701: /* fwd_stmt: "fwd" "to" stmt_expr  */
#line 4098 "src/parser_bison.y"
                        {
				(yyval.stmt) = fwd_stmt_alloc(&(yyloc));
				(yyval.stmt)->fwd.dev = (yyvsp[0].expr);
			}
#line 13014 "src/parser_bison.c"
    break;

  case 702: /* fwd_stmt: "fwd" nf_key_proto "to" stmt_expr "device" stmt_expr  */
#line 4103 "src/parser_bison.y"
                        {
				(yyval.stmt) = fwd_stmt_alloc(&(yyloc));
				(yyval.stmt)->fwd.family = (yyvsp[-4].val);
				(yyval.stmt)->fwd.addr = (yyvsp[-2].expr);
				(yyval.stmt)->fwd.dev = (yyvsp[0].expr);
			}
#line 13025 "src/parser_bison.c"
    break;

  case 704: /* nf_nat_flags: nf_nat_flags "comma" nf_nat_flag  */
#line 4113 "src/parser_bison.y"
                        {
				(yyval.val) = (yyvsp[-2].val) | (yyvsp[0].val);
			}
#line 13033 "src/parser_bison.c"
    break;

  case 705: /* nf_nat_flag: "random"  */
#line 4118 "src/parser_bison.y"
                                                { (yyval.val) = NF_NAT_RANGE_PROTO_RANDOM; }
#line 13039 "src/parser_bison.c"
    break;

  case 706: /* nf_nat_flag: "fully-random"  */
#line 4119 "src/parser_bison.y"
                                                { (yyval.val) = NF_NAT_RANGE_PROTO_RANDOM_FULLY; }
#line 13045 "src/parser_bison.c"
    break;

  case 707: /* nf_nat_flag: "persistent"  */
#line 4120 "src/parser_bison.y"
                                                { (yyval.val) = NF_NAT_RANGE_PERSISTENT; }
#line 13051 "src/parser_bison.c"
    break;

  case 709: /* queue_stmt: "queue" "to" queue_stmt_expr close_scope_queue  */
#line 4125 "src/parser_bison.y"
                        {
				(yyval.stmt) = queue_stmt_alloc(&(yyloc), (yyvsp[-1].expr), 0);
			}
#line 13059 "src/parser_bison.c"
    break;

  case 710: /* queue_stmt: "queue" "flags" queue_stmt_flags "to" queue_stmt_expr close_scope_queue  */
#line 4129 "src/parser_bison.y"
                        {
				(yyval.stmt) = queue_stmt_alloc(&(yyloc), (yyvsp[-1].expr), (yyvsp[-3].val));
			}
#line 13067 "src/parser_bison.c"
    break;

  case 711: /* queue_stmt: "queue" "flags" queue_stmt_flags "num" queue_stmt_expr_simple close_scope_queue  */
#line 4133 "src/parser_bison.y"
                        {
				(yyval.stmt) = queue_stmt_alloc(&(yyloc), (yyvsp[-1].expr), (yyvsp[-3].val));
			}
#line 13075 "src/parser_bison.c"
    break;

  case 714: /* queue_stmt_alloc: "queue"  */
#line 4143 "src/parser_bison.y"
                        {
				(yyval.stmt) = queue_stmt_alloc(&(yyloc), NULL, 0);
			}
#line 13083 "src/parser_bison.c"
    break;

  case 715: /* queue_stmt_args: queue_stmt_arg  */
#line 4149 "src/parser_bison.y"
                        {
				(yyval.stmt)	= (yyvsp[-1].stmt);
			}
#line 13091 "src/parser_bison.c"
    break;

  case 717: /* queue_stmt_arg: "num" queue_stmt_expr_simple  */
#line 4156 "src/parser_bison.y"
                        {
				(yyvsp[-2].stmt)->queue.queue = (yyvsp[0].expr);
				(yyvsp[-2].stmt)->queue.queue->location = (yyloc);
			}
#line 13100 "src/parser_bison.c"
    break;

  case 718: /* queue_stmt_arg: queue_stmt_flags  */
#line 4161 "src/parser_bison.y"
                        {
				(yyvsp[-1].stmt)->queue.flags |= (yyvsp[0].val);
			}
#line 13108 "src/parser_bison.c"
    break;

  case 723: /* queue_stmt_expr_simple: queue_expr "-" queue_expr  */
#line 4173 "src/parser_bison.y"
                        {
				(yyval.expr) = range_expr_alloc(&(yyloc), (yyvsp[-2].expr), (yyvsp[0].expr));
			}
#line 13116 "src/parser_bison.c"
    break;

  case 729: /* queue_stmt_flags: queue_stmt_flags "comma" queue_stmt_flag  */
#line 4186 "src/parser_bison.y"
                        {
				(yyval.val) = (yyvsp[-2].val) | (yyvsp[0].val);
			}
#line 13124 "src/parser_bison.c"
    break;

  case 730: /* queue_stmt_flag: "bypass"  */
#line 4191 "src/parser_bison.y"
                                        { (yyval.val) = NFT_QUEUE_FLAG_BYPASS; }
#line 13130 "src/parser_bison.c"
    break;

  case 731: /* queue_stmt_flag: "fanout"  */
#line 4192 "src/parser_bison.y"
                                        { (yyval.val) = NFT_QUEUE_FLAG_CPU_FANOUT; }
#line 13136 "src/parser_bison.c"
    break;

  case 734: /* set_elem_expr_stmt_alloc: concat_expr  */
#line 4200 "src/parser_bison.y"
                        {
				(yyval.expr) = set_elem_expr_alloc(&(yylsp[0]), (yyvsp[0].expr));
			}
#line 13144 "src/parser_bison.c"
    break;

  case 735: /* set_stmt: "set" set_stmt_op set_elem_expr_stmt set_ref_expr  */
#line 4206 "src/parser_bison.y"
                        {
				(yyval.stmt) = set_stmt_alloc(&(yyloc));
				(yyval.stmt)->set.op  = (yyvsp[-2].val);
				(yyval.stmt)->set.key = (yyvsp[-1].expr);
				(yyval.stmt)->set.set = (yyvsp[0].expr);
			}
#line 13155 "src/parser_bison.c"
    break;

  case 736: /* set_stmt: set_stmt_op set_ref_expr '{' set_elem_expr_stmt '}'  */
#line 4213 "src/parser_bison.y"
                        {
				(yyval.stmt) = set_stmt_alloc(&(yyloc));
				(yyval.stmt)->set.op  = (yyvsp[-4].val);
				(yyval.stmt)->set.key = (yyvsp[-1].expr);
				(yyval.stmt)->set.set = (yyvsp[-3].expr);
			}
#line 13166 "src/parser_bison.c"
    break;

  case 737: /* set_stmt: set_stmt_op set_ref_expr '{' set_elem_expr_stmt stateful_stmt_list '}'  */
#line 4220 "src/parser_bison.y"
                        {
				(yyval.stmt) = set_stmt_alloc(&(yyloc));
				(yyval.stmt)->set.op  = (yyvsp[-5].val);
				(yyval.stmt)->set.key = (yyvsp[-2].expr);
				(yyval.stmt)->set.set = (yyvsp[-4].expr);
				list_splice_tail((yyvsp[-1].list), &(yyval.stmt)->set.stmt_list);
				free((yyvsp[-1].list));
			}
#line 13179 "src/parser_bison.c"
    break;

  case 738: /* set_stmt_op: "add"  */
#line 4230 "src/parser_bison.y"
                                        { (yyval.val) = NFT_DYNSET_OP_ADD; }
#line 13185 "src/parser_bison.c"
    break;

  case 739: /* set_stmt_op: "update"  */
#line 4231 "src/parser_bison.y"
                                        { (yyval.val) = NFT_DYNSET_OP_UPDATE; }
#line 13191 "src/parser_bison.c"
    break;

  case 740: /* set_stmt_op: "delete"  */
#line 4232 "src/parser_bison.y"
                                        { (yyval.val) = NFT_DYNSET_OP_DELETE; }
#line 13197 "src/parser_bison.c"
    break;

  case 741: /* map_stmt: set_stmt_op set_ref_expr '{' set_elem_expr_stmt "colon" set_elem_expr_stmt '}'  */
#line 4236 "src/parser_bison.y"
                        {
				(yyval.stmt) = map_stmt_alloc(&(yyloc));
				(yyval.stmt)->map.op  = (yyvsp[-6].val);
				(yyval.stmt)->map.key = (yyvsp[-3].expr);
				(yyval.stmt)->map.data = (yyvsp[-1].expr);
				(yyval.stmt)->map.set = (yyvsp[-5].expr);
			}
#line 13209 "src/parser_bison.c"
    break;

  case 742: /* map_stmt: set_stmt_op set_ref_expr '{' set_elem_expr_stmt stateful_stmt_list "colon" set_elem_expr_stmt '}'  */
#line 4244 "src/parser_bison.y"
                        {
				(yyval.stmt) = map_stmt_alloc(&(yyloc));
				(yyval.stmt)->map.op  = (yyvsp[-7].val);
				(yyval.stmt)->map.key = (yyvsp[-4].expr);
				(yyval.stmt)->map.data = (yyvsp[-1].expr);
				(yyval.stmt)->map.set = (yyvsp[-6].expr);
				list_splice_tail((yyvsp[-3].list), &(yyval.stmt)->map.stmt_list);
				free((yyvsp[-3].list));
			}
#line 13223 "src/parser_bison.c"
    break;

  case 743: /* meter_stmt: "meter" identifier '{' meter_key_expr stmt '}'  */
#line 4256 "src/parser_bison.y"
                        {
				(yyval.stmt) = meter_stmt_alloc(&(yyloc));
				(yyval.stmt)->meter.name = (yyvsp[-4].string);
				(yyval.stmt)->meter.size = 0;
				(yyval.stmt)->meter.key  = (yyvsp[-2].expr);
				(yyval.stmt)->meter.stmt = (yyvsp[-1].stmt);
				(yyval.stmt)->location  = (yyloc);
			}
#line 13236 "src/parser_bison.c"
    break;

  case 744: /* meter_stmt: "meter" identifier "size" "number" '{' meter_key_expr stmt '}'  */
#line 4265 "src/parser_bison.y"
                        {
				(yyval.stmt) = meter_stmt_alloc(&(yyloc));
				(yyval.stmt)->meter.name = (yyvsp[-6].string);
				(yyval.stmt)->meter.size = (yyvsp[-4].val);
				(yyval.stmt)->meter.key  = (yyvsp[-2].expr);
				(yyval.stmt)->meter.stmt = (yyvsp[-1].stmt);
				(yyval.stmt)->location  = (yyloc);
			}
#line 13249 "src/parser_bison.c"
    break;

  case 745: /* match_stmt: relational_expr  */
#line 4276 "src/parser_bison.y"
                        {
				(yyval.stmt) = expr_stmt_alloc(&(yyloc), (yyvsp[0].expr));
			}
#line 13257 "src/parser_bison.c"
    break;

  case 746: /* variable_expr: '$' identifier  */
#line 4282 "src/parser_bison.y"
                        {
				struct scope *scope = current_scope(state);
				struct symbol *sym;

				sym = symbol_get(scope, (yyvsp[0].string));
				if (!sym) {
					sym = symbol_lookup_fuzzy(scope, (yyvsp[0].string));
					if (sym) {
						erec_queue(error(&(yylsp[0]), "unknown identifier '%s'; "
								      "did you mean identifier '%s’?",
								      (yyvsp[0].string), sym->identifier),
							   state->msgs);
					} else {
						erec_queue(error(&(yylsp[0]), "unknown identifier '%s'", (yyvsp[0].string)),
							   state->msgs);
					}
					free_const((yyvsp[0].string));
					YYERROR;
				}

				(yyval.expr) = variable_expr_alloc(&(yyloc), scope, sym);
				free_const((yyvsp[0].string));
			}
#line 13285 "src/parser_bison.c"
    break;

  case 748: /* symbol_expr: string  */
#line 4309 "src/parser_bison.y"
                        {
				(yyval.expr) = symbol_expr_alloc(&(yyloc), SYMBOL_VALUE,
						       current_scope(state),
						       (yyvsp[0].string));
				free_const((yyvsp[0].string));
			}
#line 13296 "src/parser_bison.c"
    break;

  case 751: /* set_ref_symbol_expr: "@" identifier close_scope_at  */
#line 4322 "src/parser_bison.y"
                        {
				(yyval.expr) = symbol_expr_alloc(&(yyloc), SYMBOL_SET,
						       current_scope(state),
						       (yyvsp[-1].string));
				free_const((yyvsp[-1].string));
			}
#line 13307 "src/parser_bison.c"
    break;

  case 752: /* integer_expr: "number"  */
#line 4331 "src/parser_bison.y"
                        {
				char str[64];

				snprintf(str, sizeof(str), "%" PRIu64, (yyvsp[0].val));
				(yyval.expr) = symbol_expr_alloc(&(yyloc), SYMBOL_VALUE,
						       current_scope(state),
						       str);
			}
#line 13320 "src/parser_bison.c"
    break;

  case 753: /* selector_expr: payload_expr  */
#line 4341 "src/parser_bison.y"
                                                                { (yyval.expr) = (yyvsp[0].expr); }
#line 13326 "src/parser_bison.c"
    break;

  case 754: /* selector_expr: exthdr_expr  */
#line 4342 "src/parser_bison.y"
                                                                { (yyval.expr) = (yyvsp[0].expr); }
#line 13332 "src/parser_bison.c"
    break;

  case 755: /* selector_expr: exthdr_exists_expr  */
#line 4343 "src/parser_bison.y"
                                                                { (yyval.expr) = (yyvsp[0].expr); }
#line 13338 "src/parser_bison.c"
    break;

  case 756: /* selector_expr: meta_expr  */
#line 4344 "src/parser_bison.y"
                                                                { (yyval.expr) = (yyvsp[0].expr); }
#line 13344 "src/parser_bison.c"
    break;

  case 757: /* selector_expr: socket_expr  */
#line 4345 "src/parser_bison.y"
                                                                { (yyval.expr) = (yyvsp[0].expr); }
#line 13350 "src/parser_bison.c"
    break;

  case 758: /* selector_expr: rt_expr  */
#line 4346 "src/parser_bison.y"
                                                                { (yyval.expr) = (yyvsp[0].expr); }
#line 13356 "src/parser_bison.c"
    break;

  case 759: /* selector_expr: ct_expr  */
#line 4347 "src/parser_bison.y"
                                                                { (yyval.expr) = (yyvsp[0].expr); }
#line 13362 "src/parser_bison.c"
    break;

  case 760: /* selector_expr: numgen_expr  */
#line 4348 "src/parser_bison.y"
                                                                { (yyval.expr) = (yyvsp[0].expr); }
#line 13368 "src/parser_bison.c"
    break;

  case 761: /* selector_expr: hash_expr  */
#line 4349 "src/parser_bison.y"
                                                                { (yyval.expr) = (yyvsp[0].expr); }
#line 13374 "src/parser_bison.c"
    break;

  case 762: /* selector_expr: fib_expr  */
#line 4350 "src/parser_bison.y"
                                                                { (yyval.expr) = (yyvsp[0].expr); }
#line 13380 "src/parser_bison.c"
    break;

  case 763: /* selector_expr: osf_expr  */
#line 4351 "src/parser_bison.y"
                                                                { (yyval.expr) = (yyvsp[0].expr); }
#line 13386 "src/parser_bison.c"
    break;

  case 764: /* selector_expr: xfrm_expr  */
#line 4352 "src/parser_bison.y"
                                                                { (yyval.expr) = (yyvsp[0].expr); }
#line 13392 "src/parser_bison.c"
    break;

  case 765: /* primary_expr: symbol_expr  */
#line 4355 "src/parser_bison.y"
                                                                { (yyval.expr) = (yyvsp[0].expr); }
#line 13398 "src/parser_bison.c"
    break;

  case 766: /* primary_expr: integer_expr  */
#line 4356 "src/parser_bison.y"
                                                                { (yyval.expr) = (yyvsp[0].expr); }
#line 13404 "src/parser_bison.c"
    break;

  case 767: /* primary_expr: selector_expr  */
#line 4357 "src/parser_bison.y"
                                                                { (yyval.expr) = (yyvsp[0].expr); }
#line 13410 "src/parser_bison.c"
    break;

  case 768: /* primary_expr: '(' basic_expr ')'  */
#line 4358 "src/parser_bison.y"
                                                                { (yyval.expr) = (yyvsp[-1].expr); }
#line 13416 "src/parser_bison.c"
    break;

  case 769: /* fib_expr: "fib" fib_tuple fib_result close_scope_fib  */
#line 4362 "src/parser_bison.y"
                        {
				if (((yyvsp[-2].val) & (NFTA_FIB_F_SADDR|NFTA_FIB_F_DADDR)) == 0) {
					erec_queue(error(&(yylsp[-2]), "fib: need either saddr or daddr"), state->msgs);
					YYERROR;
				}

				if (((yyvsp[-2].val) & (NFTA_FIB_F_SADDR|NFTA_FIB_F_DADDR)) ==
					  (NFTA_FIB_F_SADDR|NFTA_FIB_F_DADDR)) {
					erec_queue(error(&(yylsp[-2]), "fib: saddr and daddr are mutually exclusive"), state->msgs);
					YYERROR;
				}

				if (((yyvsp[-2].val) & (NFTA_FIB_F_IIF|NFTA_FIB_F_OIF)) ==
					  (NFTA_FIB_F_IIF|NFTA_FIB_F_OIF)) {
					erec_queue(error(&(yylsp[-2]), "fib: iif and oif are mutually exclusive"), state->msgs);
					YYERROR;
				}

				(yyval.expr) = fib_expr_alloc(&(yyloc), (yyvsp[-2].val), (yyvsp[-1].val));
			}
#line 13441 "src/parser_bison.c"
    break;

  case 770: /* fib_result: "oif"  */
#line 4384 "src/parser_bison.y"
                                        { (yyval.val) =NFT_FIB_RESULT_OIF; }
#line 13447 "src/parser_bison.c"
    break;

  case 771: /* fib_result: "oifname"  */
#line 4385 "src/parser_bison.y"
                                        { (yyval.val) =NFT_FIB_RESULT_OIFNAME; }
#line 13453 "src/parser_bison.c"
    break;

  case 772: /* fib_result: "type" close_scope_type  */
#line 4386 "src/parser_bison.y"
                                                                { (yyval.val) =NFT_FIB_RESULT_ADDRTYPE; }
#line 13459 "src/parser_bison.c"
    break;

  case 773: /* fib_flag: "saddr"  */
#line 4389 "src/parser_bison.y"
                                        { (yyval.val) = NFTA_FIB_F_SADDR; }
#line 13465 "src/parser_bison.c"
    break;

  case 774: /* fib_flag: "daddr"  */
#line 4390 "src/parser_bison.y"
                                        { (yyval.val) = NFTA_FIB_F_DADDR; }
#line 13471 "src/parser_bison.c"
    break;

  case 775: /* fib_flag: "mark"  */
#line 4391 "src/parser_bison.y"
                                        { (yyval.val) = NFTA_FIB_F_MARK; }
#line 13477 "src/parser_bison.c"
    break;

  case 776: /* fib_flag: "iif"  */
#line 4392 "src/parser_bison.y"
                                        { (yyval.val) = NFTA_FIB_F_IIF; }
#line 13483 "src/parser_bison.c"
    break;

  case 777: /* fib_flag: "oif"  */
#line 4393 "src/parser_bison.y"
                                        { (yyval.val) = NFTA_FIB_F_OIF; }
#line 13489 "src/parser_bison.c"
    break;

  case 778: /* fib_tuple: fib_flag "." fib_tuple  */
#line 4397 "src/parser_bison.y"
                        {
				(yyval.val) = (yyvsp[-2].val) | (yyvsp[0].val);
			}
#line 13497 "src/parser_bison.c"
    break;

  case 780: /* osf_expr: "osf" osf_ttl "version" close_scope_osf  */
#line 4404 "src/parser_bison.y"
                        {
				(yyval.expr) = osf_expr_alloc(&(yyloc), (yyvsp[-2].val), NFT_OSF_F_VERSION);
			}
#line 13505 "src/parser_bison.c"
    break;

  case 781: /* osf_expr: "osf" osf_ttl "name" close_scope_osf  */
#line 4408 "src/parser_bison.y"
                        {
				(yyval.expr) = osf_expr_alloc(&(yyloc), (yyvsp[-2].val), 0);
			}
#line 13513 "src/parser_bison.c"
    break;

  case 782: /* osf_ttl: %empty  */
#line 4414 "src/parser_bison.y"
                        {
				(yyval.val) = NF_OSF_TTL_TRUE;
			}
#line 13521 "src/parser_bison.c"
    break;

  case 783: /* osf_ttl: "ttl" "string"  */
#line 4418 "src/parser_bison.y"
                        {
				if (!strcmp((yyvsp[0].string), "loose"))
					(yyval.val) = NF_OSF_TTL_LESS;
				else if (!strcmp((yyvsp[0].string), "skip"))
					(yyval.val) = NF_OSF_TTL_NOCHECK;
				else {
					erec_queue(error(&(yylsp[0]), "invalid ttl option"),
						   state->msgs);
					free_const((yyvsp[0].string));
					YYERROR;
				}
				free_const((yyvsp[0].string));
			}
#line 13539 "src/parser_bison.c"
    break;

  case 785: /* shift_expr: shift_expr "<<" primary_rhs_expr  */
#line 4435 "src/parser_bison.y"
                        {
				(yyval.expr) = binop_expr_alloc(&(yyloc), OP_LSHIFT, (yyvsp[-2].expr), (yyvsp[0].expr));
			}
#line 13547 "src/parser_bison.c"
    break;

  case 786: /* shift_expr: shift_expr ">>" primary_rhs_expr  */
#line 4439 "src/parser_bison.y"
                        {
				(yyval.expr) = binop_expr_alloc(&(yyloc), OP_RSHIFT, (yyvsp[-2].expr), (yyvsp[0].expr));
			}
#line 13555 "src/parser_bison.c"
    break;

  case 788: /* and_expr: and_expr "&" shift_rhs_expr  */
#line 4446 "src/parser_bison.y"
                        {
				(yyval.expr) = binop_expr_alloc(&(yyloc), OP_AND, (yyvsp[-2].expr), (yyvsp[0].expr));
			}
#line 13563 "src/parser_bison.c"
    break;

  case 790: /* exclusive_or_expr: exclusive_or_expr "^" and_rhs_expr  */
#line 4453 "src/parser_bison.y"
                        {
				(yyval.expr) = binop_expr_alloc(&(yyloc), OP_XOR, (yyvsp[-2].expr), (yyvsp[0].expr));
			}
#line 13571 "src/parser_bison.c"
    break;

  case 792: /* inclusive_or_expr: inclusive_or_expr '|' exclusive_or_rhs_expr  */
#line 4460 "src/parser_bison.y"
                        {
				(yyval.expr) = binop_expr_alloc(&(yyloc), OP_OR, (yyvsp[-2].expr), (yyvsp[0].expr));
			}
#line 13579 "src/parser_bison.c"
    break;

  case 795: /* concat_expr: concat_expr "." basic_expr  */
#line 4470 "src/parser_bison.y"
                        {
				struct location rhs[] = {
					[1]	= (yylsp[-1]),
					[2]	= (yylsp[0]),
				};

				(yyval.expr) = handle_concat_expr(&(yyloc), (yyval.expr), (yyvsp[-2].expr), (yyvsp[0].expr), rhs);
			}
#line 13592 "src/parser_bison.c"
    break;

  case 796: /* prefix_rhs_expr: basic_rhs_expr "/" "number"  */
#line 4481 "src/parser_bison.y"
                        {
				(yyval.expr) = prefix_expr_alloc(&(yyloc), (yyvsp[-2].expr), (yyvsp[0].val));
			}
#line 13600 "src/parser_bison.c"
    break;

  case 797: /* range_rhs_expr: basic_rhs_expr "-" basic_rhs_expr  */
#line 4487 "src/parser_bison.y"
                        {
				if ((yyvsp[-2].expr)->etype == EXPR_SYMBOL &&
				    (yyvsp[-2].expr)->symtype == SYMBOL_VALUE &&
				    (yyvsp[0].expr)->etype == EXPR_SYMBOL &&
				    (yyvsp[0].expr)->symtype == SYMBOL_VALUE) {
					(yyval.expr) = symbol_range_expr_alloc(&(yyloc), (yyvsp[-2].expr)->symtype, (yyvsp[-2].expr)->scope, (yyvsp[-2].expr)->identifier, (yyvsp[0].expr)->identifier);
					expr_free((yyvsp[-2].expr));
					expr_free((yyvsp[0].expr));
				} else {
					(yyval.expr) = range_expr_alloc(&(yyloc), (yyvsp[-2].expr), (yyvsp[0].expr));
				}
			}
#line 13617 "src/parser_bison.c"
    break;

  case 800: /* map_expr: concat_expr "map" rhs_expr  */
#line 4506 "src/parser_bison.y"
                        {
				(yyval.expr) = map_expr_alloc(&(yyloc), (yyvsp[-2].expr), (yyvsp[0].expr));
			}
#line 13625 "src/parser_bison.c"
    break;

  case 804: /* set_expr: '{' set_list_expr '}'  */
#line 4517 "src/parser_bison.y"
                        {
				(yyvsp[-1].expr)->location = (yyloc);
				(yyval.expr) = (yyvsp[-1].expr);
			}
#line 13634 "src/parser_bison.c"
    break;

  case 805: /* set_list_expr: set_list_member_expr  */
#line 4524 "src/parser_bison.y"
                        {
				(yyval.expr) = set_expr_alloc(&(yyloc), NULL);
				compound_expr_add((yyval.expr), (yyvsp[0].expr));
			}
#line 13643 "src/parser_bison.c"
    break;

  case 806: /* set_list_expr: set_list_expr "comma" set_list_member_expr  */
#line 4529 "src/parser_bison.y"
                        {
				compound_expr_add((yyvsp[-2].expr), (yyvsp[0].expr));
				(yyval.expr) = (yyvsp[-2].expr);
			}
#line 13652 "src/parser_bison.c"
    break;

  case 808: /* set_list_member_expr: opt_newline set_expr opt_newline  */
#line 4537 "src/parser_bison.y"
                        {
				(yyval.expr) = (yyvsp[-1].expr);
			}
#line 13660 "src/parser_bison.c"
    break;

  case 809: /* set_list_member_expr: opt_newline set_elem_expr opt_newline  */
#line 4541 "src/parser_bison.y"
                        {
				(yyval.expr) = (yyvsp[-1].expr);
			}
#line 13668 "src/parser_bison.c"
    break;

  case 810: /* set_list_member_expr: opt_newline set_elem_expr "colon" set_rhs_expr opt_newline  */
#line 4545 "src/parser_bison.y"
                        {
				(yyval.expr) = mapping_expr_alloc(&(yylsp[-3]), (yyvsp[-3].expr), (yyvsp[-1].expr));
			}
#line 13676 "src/parser_bison.c"
    break;

  case 812: /* meter_key_expr: meter_key_expr_alloc set_elem_options  */
#line 4552 "src/parser_bison.y"
                        {
				(yyval.expr)->location = (yyloc);
				(yyval.expr) = (yyvsp[-1].expr);
			}
#line 13685 "src/parser_bison.c"
    break;

  case 813: /* meter_key_expr_alloc: concat_expr  */
#line 4559 "src/parser_bison.y"
                        {
				(yyval.expr) = set_elem_expr_alloc(&(yylsp[0]), (yyvsp[0].expr));
			}
#line 13693 "src/parser_bison.c"
    break;

  case 816: /* set_elem_expr: set_elem_expr_alloc set_elem_expr_options set_elem_stmt_list  */
#line 4567 "src/parser_bison.y"
                        {
				(yyval.expr) = (yyvsp[-2].expr);
				list_splice_tail((yyvsp[0].list), &(yyval.expr)->stmt_list);
				free((yyvsp[0].list));
			}
#line 13703 "src/parser_bison.c"
    break;

  case 817: /* set_elem_key_expr: set_lhs_expr  */
#line 4574 "src/parser_bison.y"
                                                        { (yyval.expr) = (yyvsp[0].expr); }
#line 13709 "src/parser_bison.c"
    break;

  case 818: /* set_elem_key_expr: "*"  */
#line 4575 "src/parser_bison.y"
                                                        { (yyval.expr) = set_elem_catchall_expr_alloc(&(yylsp[0])); }
#line 13715 "src/parser_bison.c"
    break;

  case 819: /* set_elem_expr_alloc: set_elem_key_expr set_elem_stmt_list  */
#line 4579 "src/parser_bison.y"
                        {
				(yyval.expr) = set_elem_expr_alloc(&(yylsp[-1]), (yyvsp[-1].expr));
				list_splice_tail((yyvsp[0].list), &(yyval.expr)->stmt_list);
				free((yyvsp[0].list));
			}
#line 13725 "src/parser_bison.c"
    break;

  case 820: /* set_elem_expr_alloc: set_elem_key_expr  */
#line 4585 "src/parser_bison.y"
                        {
				(yyval.expr) = set_elem_expr_alloc(&(yylsp[0]), (yyvsp[0].expr));
			}
#line 13733 "src/parser_bison.c"
    break;

  case 821: /* set_elem_options: set_elem_option  */
#line 4591 "src/parser_bison.y"
                        {
				(yyval.expr)	= (yyvsp[-1].expr);
			}
#line 13741 "src/parser_bison.c"
    break;

  case 823: /* set_elem_time_spec: "string"  */
#line 4598 "src/parser_bison.y"
                        {
				struct error_record *erec;
				uint64_t res;

				if (!strcmp("never", (yyvsp[0].string))) {
					free_const((yyvsp[0].string));
					(yyval.val) = NFT_NEVER_TIMEOUT;
					break;
				}

				erec = time_parse(&(yylsp[0]), (yyvsp[0].string), &res);
				free_const((yyvsp[0].string));
				if (erec != NULL) {
					erec_queue(erec, state->msgs);
					YYERROR;
				}
				(yyval.val) = res;
			}
#line 13764 "src/parser_bison.c"
    break;

  case 824: /* set_elem_option: "timeout" time_spec  */
#line 4619 "src/parser_bison.y"
                        {
				(yyvsp[-2].expr)->timeout = (yyvsp[0].val);
			}
#line 13772 "src/parser_bison.c"
    break;

  case 825: /* set_elem_option: "expires" time_spec  */
#line 4623 "src/parser_bison.y"
                        {
				(yyvsp[-2].expr)->expiration = (yyvsp[0].val);
			}
#line 13780 "src/parser_bison.c"
    break;

  case 826: /* set_elem_option: comment_spec  */
#line 4627 "src/parser_bison.y"
                        {
				if (already_set((yyvsp[-1].expr)->comment, &(yylsp[0]), state)) {
					free_const((yyvsp[0].string));
					YYERROR;
				}
				(yyvsp[-1].expr)->comment = (yyvsp[0].string);
			}
#line 13792 "src/parser_bison.c"
    break;

  case 827: /* set_elem_expr_options: set_elem_expr_option  */
#line 4637 "src/parser_bison.y"
                        {
				(yyval.expr)	= (yyvsp[-1].expr);
			}
#line 13800 "src/parser_bison.c"
    break;

  case 829: /* set_elem_stmt_list: set_elem_stmt  */
#line 4644 "src/parser_bison.y"
                        {
				(yyval.list) = xmalloc(sizeof(*(yyval.list)));
				init_list_head((yyval.list));
				list_add_tail(&(yyvsp[0].stmt)->list, (yyval.list));
			}
#line 13810 "src/parser_bison.c"
    break;

  case 830: /* set_elem_stmt_list: set_elem_stmt_list set_elem_stmt  */
#line 4650 "src/parser_bison.y"
                        {
				(yyval.list) = (yyvsp[-1].list);
				list_add_tail(&(yyvsp[0].stmt)->list, (yyvsp[-1].list));
			}
#line 13819 "src/parser_bison.c"
    break;

  case 836: /* set_elem_expr_option: "timeout" set_elem_time_spec  */
#line 4664 "src/parser_bison.y"
                        {
				(yyvsp[-2].expr)->timeout = (yyvsp[0].val);
			}
#line 13827 "src/parser_bison.c"
    break;

  case 837: /* set_elem_expr_option: "expires" time_spec  */
#line 4668 "src/parser_bison.y"
                        {
				(yyvsp[-2].expr)->expiration = (yyvsp[0].val);
			}
#line 13835 "src/parser_bison.c"
    break;

  case 838: /* set_elem_expr_option: comment_spec  */
#line 4672 "src/parser_bison.y"
                        {
				if (already_set((yyvsp[-1].expr)->comment, &(yylsp[0]), state)) {
					free_const((yyvsp[0].string));
					YYERROR;
				}
				(yyvsp[-1].expr)->comment = (yyvsp[0].string);
			}
#line 13847 "src/parser_bison.c"
    break;

  case 844: /* initializer_expr: '{' '}'  */
#line 4690 "src/parser_bison.y"
                                                { (yyval.expr) = compound_expr_alloc(&(yyloc), EXPR_SET); }
#line 13853 "src/parser_bison.c"
    break;

  case 845: /* initializer_expr: "-" "number"  */
#line 4692 "src/parser_bison.y"
                        {
				int32_t num = -(yyvsp[0].val);

				(yyval.expr) = constant_expr_alloc(&(yyloc), &integer_type,
							 BYTEORDER_HOST_ENDIAN,
							 sizeof(num) * BITS_PER_BYTE,
							 &num);
			}
#line 13866 "src/parser_bison.c"
    break;

  case 846: /* counter_config: "packets" "number" "bytes" "number"  */
#line 4703 "src/parser_bison.y"
                        {
				struct counter *counter;

				counter = &(yyvsp[-4].obj)->counter;
				counter->packets = (yyvsp[-2].val);
				counter->bytes = (yyvsp[0].val);
			}
#line 13878 "src/parser_bison.c"
    break;

  case 847: /* counter_obj: %empty  */
#line 4713 "src/parser_bison.y"
                        {
				(yyval.obj) = obj_alloc(&(yyloc));
				(yyval.obj)->type = NFT_OBJECT_COUNTER;
			}
#line 13887 "src/parser_bison.c"
    break;

  case 848: /* quota_config: quota_mode "number" quota_unit quota_used  */
#line 4720 "src/parser_bison.y"
                        {
				struct error_record *erec;
				struct quota *quota;
				uint64_t rate;

				erec = data_unit_parse(&(yyloc), (yyvsp[-1].string), &rate);
				free_const((yyvsp[-1].string));
				if (erec != NULL) {
					erec_queue(erec, state->msgs);
					YYERROR;
				}

				quota = &(yyvsp[-4].obj)->quota;
				quota->bytes	= (yyvsp[-2].val) * rate;
				quota->used	= (yyvsp[0].val);
				quota->flags	= (yyvsp[-3].val);
			}
#line 13909 "src/parser_bison.c"
    break;

  case 849: /* quota_obj: %empty  */
#line 4740 "src/parser_bison.y"
                        {
				(yyval.obj) = obj_alloc(&(yyloc));
				(yyval.obj)->type = NFT_OBJECT_QUOTA;
			}
#line 13918 "src/parser_bison.c"
    break;

  case 850: /* secmark_config: string  */
#line 4747 "src/parser_bison.y"
                        {
				int ret;
				struct secmark *secmark;

				secmark = &(yyvsp[-1].obj)->secmark;
				ret = snprintf(secmark->ctx, sizeof(secmark->ctx), "%s", (yyvsp[0].string));
				if (ret <= 0 || ret >= (int)sizeof(secmark->ctx)) {
					erec_queue(error(&(yylsp[0]), "invalid context '%s', max length is %u\n", (yyvsp[0].string), (int)sizeof(secmark->ctx)), state->msgs);
					free_const((yyvsp[0].string));
					YYERROR;
				}
				free_const((yyvsp[0].string));
			}
#line 13936 "src/parser_bison.c"
    break;

  case 851: /* secmark_obj: %empty  */
#line 4763 "src/parser_bison.y"
                        {
				(yyval.obj) = obj_alloc(&(yyloc));
				(yyval.obj)->type = NFT_OBJECT_SECMARK;
			}
#line 13945 "src/parser_bison.c"
    break;

  case 852: /* ct_obj_type: "helper"  */
#line 4769 "src/parser_bison.y"
                                                { (yyval.val) = NFT_OBJECT_CT_HELPER; }
#line 13951 "src/parser_bison.c"
    break;

  case 853: /* ct_obj_type: "timeout"  */
#line 4770 "src/parser_bison.y"
                                                { (yyval.val) = NFT_OBJECT_CT_TIMEOUT; }
#line 13957 "src/parser_bison.c"
    break;

  case 854: /* ct_obj_type: "expectation"  */
#line 4771 "src/parser_bison.y"
                                                { (yyval.val) = NFT_OBJECT_CT_EXPECT; }
#line 13963 "src/parser_bison.c"
    break;

  case 855: /* ct_cmd_type: "helpers"  */
#line 4774 "src/parser_bison.y"
                                                { (yyval.val) = CMD_OBJ_CT_HELPERS; }
#line 13969 "src/parser_bison.c"
    break;

  case 856: /* ct_cmd_type: "timeout"  */
#line 4775 "src/parser_bison.y"
                                                { (yyval.val) = CMD_OBJ_CT_TIMEOUTS; }
#line 13975 "src/parser_bison.c"
    break;

  case 857: /* ct_cmd_type: "expectation"  */
#line 4776 "src/parser_bison.y"
                                                { (yyval.val) = CMD_OBJ_CT_EXPECTATIONS; }
#line 13981 "src/parser_bison.c"
    break;

  case 858: /* ct_l4protoname: "tcp" close_scope_tcp  */
#line 4779 "src/parser_bison.y"
                                                        { (yyval.val) = IPPROTO_TCP; }
#line 13987 "src/parser_bison.c"
    break;

  case 859: /* ct_l4protoname: "udp" close_scope_udp  */
#line 4780 "src/parser_bison.y"
                                                        { (yyval.val) = IPPROTO_UDP; }
#line 13993 "src/parser_bison.c"
    break;

  case 860: /* ct_helper_config: "type" "quoted string" "protocol" ct_l4protoname stmt_separator close_scope_type  */
#line 4784 "src/parser_bison.y"
                        {
				struct ct_helper *ct;
				int ret;

				ct = &(yyvsp[-6].obj)->ct_helper;

				if (ct->l4proto) {
					erec_queue(error(&(yylsp[-4]), "You can only specify this once. This statement is already set for %s.", ct->name), state->msgs);
					free_const((yyvsp[-4].string));
					YYERROR;
				}

				ret = snprintf(ct->name, sizeof(ct->name), "%s", (yyvsp[-4].string));
				if (ret <= 0 || ret >= (int)sizeof(ct->name)) {
					erec_queue(error(&(yylsp[-4]), "invalid name '%s', max length is %u\n", (yyvsp[-4].string), (int)sizeof(ct->name)), state->msgs);
					free_const((yyvsp[-4].string));
					YYERROR;
				}
				free_const((yyvsp[-4].string));

				ct->l4proto = (yyvsp[-2].val);
			}
#line 14020 "src/parser_bison.c"
    break;

  case 861: /* ct_helper_config: "l3proto" family_spec_explicit stmt_separator  */
#line 4807 "src/parser_bison.y"
                        {
				(yyvsp[-3].obj)->ct_helper.l3proto = (yyvsp[-1].val);
			}
#line 14028 "src/parser_bison.c"
    break;

  case 862: /* timeout_states: timeout_state  */
#line 4813 "src/parser_bison.y"
                        {
				(yyval.list) = xmalloc(sizeof(*(yyval.list)));
				init_list_head((yyval.list));
				list_add_tail(&(yyvsp[0].timeout_state)->head, (yyval.list));
			}
#line 14038 "src/parser_bison.c"
    break;

  case 863: /* timeout_states: timeout_states "comma" timeout_state  */
#line 4819 "src/parser_bison.y"
                        {
				list_add_tail(&(yyvsp[0].timeout_state)->head, (yyvsp[-2].list));
				(yyval.list) = (yyvsp[-2].list);
			}
#line 14047 "src/parser_bison.c"
    break;

  case 864: /* timeout_state: "string" "colon" time_spec_or_num_s  */
#line 4826 "src/parser_bison.y"
                        {
				struct timeout_state *ts;

				ts = xzalloc(sizeof(*ts));
				ts->timeout_str = (yyvsp[-2].string);
				ts->timeout_value = (yyvsp[0].val);
				ts->location = (yylsp[-2]);
				init_list_head(&ts->head);
				(yyval.timeout_state) = ts;
			}
#line 14062 "src/parser_bison.c"
    break;

  case 865: /* ct_timeout_config: "protocol" ct_l4protoname stmt_separator  */
#line 4839 "src/parser_bison.y"
                        {
				struct ct_timeout *ct;
				int l4proto = (yyvsp[-1].val);

				ct = &(yyvsp[-3].obj)->ct_timeout;
				ct->l4proto = l4proto;
			}
#line 14074 "src/parser_bison.c"
    break;

  case 866: /* ct_timeout_config: "policy" '=' '{' timeout_states '}' stmt_separator close_scope_policy  */
#line 4847 "src/parser_bison.y"
                        {
				struct ct_timeout *ct;

				ct = &(yyvsp[-7].obj)->ct_timeout;
				list_splice_tail((yyvsp[-3].list), &ct->timeout_list);
				free((yyvsp[-3].list));
			}
#line 14086 "src/parser_bison.c"
    break;

  case 867: /* ct_timeout_config: "l3proto" family_spec_explicit stmt_separator  */
#line 4855 "src/parser_bison.y"
                        {
				(yyvsp[-3].obj)->ct_timeout.l3proto = (yyvsp[-1].val);
			}
#line 14094 "src/parser_bison.c"
    break;

  case 868: /* ct_expect_config: "protocol" ct_l4protoname stmt_separator  */
#line 4861 "src/parser_bison.y"
                        {
				(yyvsp[-3].obj)->ct_expect.l4proto = (yyvsp[-1].val);
			}
#line 14102 "src/parser_bison.c"
    break;

  case 869: /* ct_expect_config: "dport" "number" stmt_separator  */
#line 4865 "src/parser_bison.y"
                        {
				(yyvsp[-3].obj)->ct_expect.dport = (yyvsp[-1].val);
			}
#line 14110 "src/parser_bison.c"
    break;

  case 870: /* ct_expect_config: "timeout" time_spec stmt_separator  */
#line 4869 "src/parser_bison.y"
                        {
				(yyvsp[-3].obj)->ct_expect.timeout = (yyvsp[-1].val);
			}
#line 14118 "src/parser_bison.c"
    break;

  case 871: /* ct_expect_config: "size" "number" stmt_separator  */
#line 4873 "src/parser_bison.y"
                        {
				(yyvsp[-3].obj)->ct_expect.size = (yyvsp[-1].val);
			}
#line 14126 "src/parser_bison.c"
    break;

  case 872: /* ct_expect_config: "l3proto" family_spec_explicit stmt_separator  */
#line 4877 "src/parser_bison.y"
                        {
				(yyvsp[-3].obj)->ct_expect.l3proto = (yyvsp[-1].val);
			}
#line 14134 "src/parser_bison.c"
    break;

  case 873: /* ct_obj_alloc: %empty  */
#line 4883 "src/parser_bison.y"
                        {
				(yyval.obj) = obj_alloc(&(yyloc));
			}
#line 14142 "src/parser_bison.c"
    break;

  case 874: /* limit_config: "rate" limit_mode limit_rate_pkts limit_burst_pkts  */
#line 4889 "src/parser_bison.y"
                        {
				struct limit *limit;

				limit = &(yyvsp[-4].obj)->limit;
				limit->rate	= (yyvsp[-1].limit_rate).rate;
				limit->unit	= (yyvsp[-1].limit_rate).unit;
				limit->burst	= (yyvsp[0].val);
				limit->type	= NFT_LIMIT_PKTS;
				limit->flags	= (yyvsp[-2].val);
			}
#line 14157 "src/parser_bison.c"
    break;

  case 875: /* limit_config: "rate" limit_mode limit_rate_bytes limit_burst_bytes  */
#line 4900 "src/parser_bison.y"
                        {
				struct limit *limit;

				limit = &(yyvsp[-4].obj)->limit;
				limit->rate	= (yyvsp[-1].limit_rate).rate;
				limit->unit	= (yyvsp[-1].limit_rate).unit;
				limit->burst	= (yyvsp[0].val);
				limit->type	= NFT_LIMIT_PKT_BYTES;
				limit->flags	= (yyvsp[-2].val);
			}
#line 14172 "src/parser_bison.c"
    break;

  case 876: /* limit_obj: %empty  */
#line 4913 "src/parser_bison.y"
                        {
				(yyval.obj) = obj_alloc(&(yyloc));
				(yyval.obj)->type = NFT_OBJECT_LIMIT;
			}
#line 14181 "src/parser_bison.c"
    break;

  case 877: /* relational_expr: expr rhs_expr  */
#line 4920 "src/parser_bison.y"
                        {
				(yyval.expr) = relational_expr_alloc(&(yyloc), OP_IMPLICIT, (yyvsp[-1].expr), (yyvsp[0].expr));
			}
#line 14189 "src/parser_bison.c"
    break;

  case 878: /* relational_expr: expr list_rhs_expr  */
#line 4924 "src/parser_bison.y"
                        {
				(yyval.expr) = relational_expr_alloc(&(yyloc), OP_IMPLICIT, (yyvsp[-1].expr), (yyvsp[0].expr));
			}
#line 14197 "src/parser_bison.c"
    break;

  case 879: /* relational_expr: expr basic_rhs_expr "/" list_rhs_expr  */
#line 4928 "src/parser_bison.y"
                        {
				struct expr *mask = list_expr_to_binop((yyvsp[0].expr));
				struct expr *binop = binop_expr_alloc(&(yyloc), OP_AND, (yyvsp[-3].expr), mask);

				(yyval.expr) = relational_expr_alloc(&(yyloc), OP_IMPLICIT, binop, (yyvsp[-2].expr));
			}
#line 14208 "src/parser_bison.c"
    break;

  case 880: /* relational_expr: expr list_rhs_expr "/" list_rhs_expr  */
#line 4935 "src/parser_bison.y"
                        {
				struct expr *value = list_expr_to_binop((yyvsp[-2].expr));
				struct expr *mask = list_expr_to_binop((yyvsp[0].expr));
				struct expr *binop = binop_expr_alloc(&(yyloc), OP_AND, (yyvsp[-3].expr), mask);

				(yyval.expr) = relational_expr_alloc(&(yyloc), OP_IMPLICIT, binop, value);
			}
#line 14220 "src/parser_bison.c"
    break;

  case 881: /* relational_expr: expr relational_op basic_rhs_expr "/" list_rhs_expr  */
#line 4943 "src/parser_bison.y"
                        {
				struct expr *mask = list_expr_to_binop((yyvsp[0].expr));
				struct expr *binop = binop_expr_alloc(&(yyloc), OP_AND, (yyvsp[-4].expr), mask);

				(yyval.expr) = relational_expr_alloc(&(yyloc), (yyvsp[-3].val), binop, (yyvsp[-2].expr));
			}
#line 14231 "src/parser_bison.c"
    break;

  case 882: /* relational_expr: expr relational_op list_rhs_expr "/" list_rhs_expr  */
#line 4950 "src/parser_bison.y"
                        {
				struct expr *value = list_expr_to_binop((yyvsp[-2].expr));
				struct expr *mask = list_expr_to_binop((yyvsp[0].expr));
				struct expr *binop = binop_expr_alloc(&(yyloc), OP_AND, (yyvsp[-4].expr), mask);

				(yyval.expr) = relational_expr_alloc(&(yyloc), (yyvsp[-3].val), binop, value);
			}
#line 14243 "src/parser_bison.c"
    break;

  case 883: /* relational_expr: expr relational_op rhs_expr  */
#line 4958 "src/parser_bison.y"
                        {
				(yyval.expr) = relational_expr_alloc(&(yylsp[-1]), (yyvsp[-1].val), (yyvsp[-2].expr), (yyvsp[0].expr));
			}
#line 14251 "src/parser_bison.c"
    break;

  case 884: /* relational_expr: expr relational_op list_rhs_expr  */
#line 4962 "src/parser_bison.y"
                        {
				(yyval.expr) = relational_expr_alloc(&(yylsp[-1]), (yyvsp[-1].val), (yyvsp[-2].expr), (yyvsp[0].expr));
			}
#line 14259 "src/parser_bison.c"
    break;

  case 885: /* list_rhs_expr: basic_rhs_expr "comma" basic_rhs_expr  */
#line 4968 "src/parser_bison.y"
                        {
				(yyval.expr) = list_expr_alloc(&(yyloc));
				compound_expr_add((yyval.expr), (yyvsp[-2].expr));
				compound_expr_add((yyval.expr), (yyvsp[0].expr));
			}
#line 14269 "src/parser_bison.c"
    break;

  case 886: /* list_rhs_expr: list_rhs_expr "comma" basic_rhs_expr  */
#line 4974 "src/parser_bison.y"
                        {
				(yyvsp[-2].expr)->location = (yyloc);
				compound_expr_add((yyvsp[-2].expr), (yyvsp[0].expr));
				(yyval.expr) = (yyvsp[-2].expr);
			}
#line 14279 "src/parser_bison.c"
    break;

  case 887: /* rhs_expr: concat_rhs_expr  */
#line 4981 "src/parser_bison.y"
                                                        { (yyval.expr) = (yyvsp[0].expr); }
#line 14285 "src/parser_bison.c"
    break;

  case 888: /* rhs_expr: set_expr  */
#line 4982 "src/parser_bison.y"
                                                        { (yyval.expr) = (yyvsp[0].expr); }
#line 14291 "src/parser_bison.c"
    break;

  case 889: /* rhs_expr: set_ref_symbol_expr  */
#line 4983 "src/parser_bison.y"
                                                        { (yyval.expr) = (yyvsp[0].expr); }
#line 14297 "src/parser_bison.c"
    break;

  case 891: /* shift_rhs_expr: shift_rhs_expr "<<" primary_rhs_expr  */
#line 4988 "src/parser_bison.y"
                        {
				(yyval.expr) = binop_expr_alloc(&(yyloc), OP_LSHIFT, (yyvsp[-2].expr), (yyvsp[0].expr));
			}
#line 14305 "src/parser_bison.c"
    break;

  case 892: /* shift_rhs_expr: shift_rhs_expr ">>" primary_rhs_expr  */
#line 4992 "src/parser_bison.y"
                        {
				(yyval.expr) = binop_expr_alloc(&(yyloc), OP_RSHIFT, (yyvsp[-2].expr), (yyvsp[0].expr));
			}
#line 14313 "src/parser_bison.c"
    break;

  case 894: /* and_rhs_expr: and_rhs_expr "&" shift_rhs_expr  */
#line 4999 "src/parser_bison.y"
                        {
				(yyval.expr) = binop_expr_alloc(&(yyloc), OP_AND, (yyvsp[-2].expr), (yyvsp[0].expr));
			}
#line 14321 "src/parser_bison.c"
    break;

  case 896: /* exclusive_or_rhs_expr: exclusive_or_rhs_expr "^" and_rhs_expr  */
#line 5006 "src/parser_bison.y"
                        {
				(yyval.expr) = binop_expr_alloc(&(yyloc), OP_XOR, (yyvsp[-2].expr), (yyvsp[0].expr));
			}
#line 14329 "src/parser_bison.c"
    break;

  case 898: /* inclusive_or_rhs_expr: inclusive_or_rhs_expr '|' exclusive_or_rhs_expr  */
#line 5013 "src/parser_bison.y"
                        {
				(yyval.expr) = binop_expr_alloc(&(yyloc), OP_OR, (yyvsp[-2].expr), (yyvsp[0].expr));
			}
#line 14337 "src/parser_bison.c"
    break;

  case 902: /* concat_rhs_expr: concat_rhs_expr "." multiton_rhs_expr  */
#line 5024 "src/parser_bison.y"
                        {
				struct location rhs[] = {
					[1]	= (yylsp[-1]),
					[2]	= (yylsp[0]),
				};

				(yyval.expr) = handle_concat_expr(&(yyloc), (yyval.expr), (yyvsp[-2].expr), (yyvsp[0].expr), rhs);
			}
#line 14350 "src/parser_bison.c"
    break;

  case 903: /* concat_rhs_expr: concat_rhs_expr "." basic_rhs_expr  */
#line 5033 "src/parser_bison.y"
                        {
				struct location rhs[] = {
					[1]	= (yylsp[-1]),
					[2]	= (yylsp[0]),
				};

				(yyval.expr) = handle_concat_expr(&(yyloc), (yyval.expr), (yyvsp[-2].expr), (yyvsp[0].expr), rhs);
			}
#line 14363 "src/parser_bison.c"
    break;

  case 904: /* boolean_keys: "exists"  */
#line 5043 "src/parser_bison.y"
                                                { (yyval.val8) = true; }
#line 14369 "src/parser_bison.c"
    break;

  case 905: /* boolean_keys: "missing"  */
#line 5044 "src/parser_bison.y"
                                                { (yyval.val8) = false; }
#line 14375 "src/parser_bison.c"
    break;

  case 906: /* boolean_expr: boolean_keys  */
#line 5048 "src/parser_bison.y"
                        {
				(yyval.expr) = constant_expr_alloc(&(yyloc), &boolean_type,
							 BYTEORDER_HOST_ENDIAN,
							 sizeof((yyvsp[0].val8)) * BITS_PER_BYTE, &(yyvsp[0].val8));
			}
#line 14385 "src/parser_bison.c"
    break;

  case 907: /* keyword_expr: "ether" close_scope_eth  */
#line 5055 "src/parser_bison.y"
                                                        { (yyval.expr) = symbol_value(&(yyloc), "ether"); }
#line 14391 "src/parser_bison.c"
    break;

  case 908: /* keyword_expr: "ip" close_scope_ip  */
#line 5056 "src/parser_bison.y"
                                                        { (yyval.expr) = symbol_value(&(yyloc), "ip"); }
#line 14397 "src/parser_bison.c"
    break;

  case 909: /* keyword_expr: "ip6" close_scope_ip6  */
#line 5057 "src/parser_bison.y"
                                                        { (yyval.expr) = symbol_value(&(yyloc), "ip6"); }
#line 14403 "src/parser_bison.c"
    break;

  case 910: /* keyword_expr: "vlan" close_scope_vlan  */
#line 5058 "src/parser_bison.y"
                                                         { (yyval.expr) = symbol_value(&(yyloc), "vlan"); }
#line 14409 "src/parser_bison.c"
    break;

  case 911: /* keyword_expr: "arp" close_scope_arp  */
#line 5059 "src/parser_bison.y"
                                                        { (yyval.expr) = symbol_value(&(yyloc), "arp"); }
#line 14415 "src/parser_bison.c"
    break;

  case 912: /* keyword_expr: "dnat" close_scope_nat  */
#line 5060 "src/parser_bison.y"
                                                        { (yyval.expr) = symbol_value(&(yyloc), "dnat"); }
#line 14421 "src/parser_bison.c"
    break;

  case 913: /* keyword_expr: "snat" close_scope_nat  */
#line 5061 "src/parser_bison.y"
                                                        { (yyval.expr) = symbol_value(&(yyloc), "snat"); }
#line 14427 "src/parser_bison.c"
    break;

  case 914: /* keyword_expr: "ecn"  */
#line 5062 "src/parser_bison.y"
                                                        { (yyval.expr) = symbol_value(&(yyloc), "ecn"); }
#line 14433 "src/parser_bison.c"
    break;

  case 915: /* keyword_expr: "reset" close_scope_reset  */
#line 5063 "src/parser_bison.y"
                                                                { (yyval.expr) = symbol_value(&(yyloc), "reset"); }
#line 14439 "src/parser_bison.c"
    break;

  case 916: /* keyword_expr: "destroy" close_scope_destroy  */
#line 5064 "src/parser_bison.y"
                                                                { (yyval.expr) = symbol_value(&(yyloc), "destroy"); }
#line 14445 "src/parser_bison.c"
    break;

  case 917: /* keyword_expr: "original"  */
#line 5065 "src/parser_bison.y"
                                                        { (yyval.expr) = symbol_value(&(yyloc), "original"); }
#line 14451 "src/parser_bison.c"
    break;

  case 918: /* keyword_expr: "reply"  */
#line 5066 "src/parser_bison.y"
                                                        { (yyval.expr) = symbol_value(&(yyloc), "reply"); }
#line 14457 "src/parser_bison.c"
    break;

  case 919: /* keyword_expr: "label"  */
#line 5067 "src/parser_bison.y"
                                                        { (yyval.expr) = symbol_value(&(yyloc), "label"); }
#line 14463 "src/parser_bison.c"
    break;

  case 920: /* keyword_expr: "last" close_scope_last  */
#line 5068 "src/parser_bison.y"
                                                                { (yyval.expr) = symbol_value(&(yyloc), "last"); }
#line 14469 "src/parser_bison.c"
    break;

  case 921: /* primary_rhs_expr: symbol_expr  */
#line 5071 "src/parser_bison.y"
                                                        { (yyval.expr) = (yyvsp[0].expr); }
#line 14475 "src/parser_bison.c"
    break;

  case 922: /* primary_rhs_expr: integer_expr  */
#line 5072 "src/parser_bison.y"
                                                        { (yyval.expr) = (yyvsp[0].expr); }
#line 14481 "src/parser_bison.c"
    break;

  case 923: /* primary_rhs_expr: boolean_expr  */
#line 5073 "src/parser_bison.y"
                                                        { (yyval.expr) = (yyvsp[0].expr); }
#line 14487 "src/parser_bison.c"
    break;

  case 924: /* primary_rhs_expr: keyword_expr  */
#line 5074 "src/parser_bison.y"
                                                        { (yyval.expr) = (yyvsp[0].expr); }
#line 14493 "src/parser_bison.c"
    break;

  case 925: /* primary_rhs_expr: "tcp" close_scope_tcp  */
#line 5076 "src/parser_bison.y"
                        {
				uint8_t data = IPPROTO_TCP;
				(yyval.expr) = constant_expr_alloc(&(yyloc), &inet_protocol_type,
							 BYTEORDER_HOST_ENDIAN,
							 sizeof(data) * BITS_PER_BYTE, &data);
			}
#line 14504 "src/parser_bison.c"
    break;

  case 926: /* primary_rhs_expr: "udp" close_scope_udp  */
#line 5083 "src/parser_bison.y"
                        {
				uint8_t data = IPPROTO_UDP;
				(yyval.expr) = constant_expr_alloc(&(yyloc), &inet_protocol_type,
							 BYTEORDER_HOST_ENDIAN,
							 sizeof(data) * BITS_PER_BYTE, &data);
			}
#line 14515 "src/parser_bison.c"
    break;

  case 927: /* primary_rhs_expr: "udplite" close_scope_udplite  */
#line 5090 "src/parser_bison.y"
                        {
				uint8_t data = IPPROTO_UDPLITE;
				(yyval.expr) = constant_expr_alloc(&(yyloc), &inet_protocol_type,
							 BYTEORDER_HOST_ENDIAN,
							 sizeof(data) * BITS_PER_BYTE, &data);
			}
#line 14526 "src/parser_bison.c"
    break;

  case 928: /* primary_rhs_expr: "esp" close_scope_esp  */
#line 5097 "src/parser_bison.y"
                        {
				uint8_t data = IPPROTO_ESP;
				(yyval.expr) = constant_expr_alloc(&(yyloc), &inet_protocol_type,
							 BYTEORDER_HOST_ENDIAN,
							 sizeof(data) * BITS_PER_BYTE, &data);
			}
#line 14537 "src/parser_bison.c"
    break;

  case 929: /* primary_rhs_expr: "ah" close_scope_ah  */
#line 5104 "src/parser_bison.y"
                        {
				uint8_t data = IPPROTO_AH;
				(yyval.expr) = constant_expr_alloc(&(yyloc), &inet_protocol_type,
							 BYTEORDER_HOST_ENDIAN,
							 sizeof(data) * BITS_PER_BYTE, &data);
			}
#line 14548 "src/parser_bison.c"
    break;

  case 930: /* primary_rhs_expr: "icmp" close_scope_icmp  */
#line 5111 "src/parser_bison.y"
                        {
				uint8_t data = IPPROTO_ICMP;
				(yyval.expr) = constant_expr_alloc(&(yyloc), &inet_protocol_type,
							 BYTEORDER_HOST_ENDIAN,
							 sizeof(data) * BITS_PER_BYTE, &data);
			}
#line 14559 "src/parser_bison.c"
    break;

  case 931: /* primary_rhs_expr: "igmp"  */
#line 5118 "src/parser_bison.y"
                        {
				uint8_t data = IPPROTO_IGMP;
				(yyval.expr) = constant_expr_alloc(&(yyloc), &inet_protocol_type,
							 BYTEORDER_HOST_ENDIAN,
							 sizeof(data) * BITS_PER_BYTE, &data);
			}
#line 14570 "src/parser_bison.c"
    break;

  case 932: /* primary_rhs_expr: "icmpv6" close_scope_icmp  */
#line 5125 "src/parser_bison.y"
                        {
				uint8_t data = IPPROTO_ICMPV6;
				(yyval.expr) = constant_expr_alloc(&(yyloc), &inet_protocol_type,
							 BYTEORDER_HOST_ENDIAN,
							 sizeof(data) * BITS_PER_BYTE, &data);
			}
#line 14581 "src/parser_bison.c"
    break;

  case 933: /* primary_rhs_expr: "gre" close_scope_gre  */
#line 5132 "src/parser_bison.y"
                        {
				uint8_t data = IPPROTO_GRE;
				(yyval.expr) = constant_expr_alloc(&(yyloc), &inet_protocol_type,
							 BYTEORDER_HOST_ENDIAN,
							 sizeof(data) * BITS_PER_BYTE, &data);
			}
#line 14592 "src/parser_bison.c"
    break;

  case 934: /* primary_rhs_expr: "comp" close_scope_comp  */
#line 5139 "src/parser_bison.y"
                        {
				uint8_t data = IPPROTO_COMP;
				(yyval.expr) = constant_expr_alloc(&(yyloc), &inet_protocol_type,
							 BYTEORDER_HOST_ENDIAN,
							 sizeof(data) * BITS_PER_BYTE, &data);
			}
#line 14603 "src/parser_bison.c"
    break;

  case 935: /* primary_rhs_expr: "dccp" close_scope_dccp  */
#line 5146 "src/parser_bison.y"
                        {
				uint8_t data = IPPROTO_DCCP;
				(yyval.expr) = constant_expr_alloc(&(yyloc), &inet_protocol_type,
							 BYTEORDER_HOST_ENDIAN,
							 sizeof(data) * BITS_PER_BYTE, &data);
			}
#line 14614 "src/parser_bison.c"
    break;

  case 936: /* primary_rhs_expr: "sctp" close_scope_sctp  */
#line 5153 "src/parser_bison.y"
                        {
				uint8_t data = IPPROTO_SCTP;
				(yyval.expr) = constant_expr_alloc(&(yyloc), &inet_protocol_type,
							 BYTEORDER_HOST_ENDIAN,
							 sizeof(data) * BITS_PER_BYTE, &data);
			}
#line 14625 "src/parser_bison.c"
    break;

  case 937: /* primary_rhs_expr: "redirect" close_scope_nat  */
#line 5160 "src/parser_bison.y"
                        {
				uint8_t data = ICMP_REDIRECT;
				(yyval.expr) = constant_expr_alloc(&(yyloc), &icmp_type_type,
							 BYTEORDER_HOST_ENDIAN,
							 sizeof(data) * BITS_PER_BYTE, &data);
			}
#line 14636 "src/parser_bison.c"
    break;

  case 938: /* primary_rhs_expr: '(' basic_rhs_expr ')'  */
#line 5166 "src/parser_bison.y"
                                                                { (yyval.expr) = (yyvsp[-1].expr); }
#line 14642 "src/parser_bison.c"
    break;

  case 939: /* relational_op: "=="  */
#line 5169 "src/parser_bison.y"
                                                { (yyval.val) = OP_EQ; }
#line 14648 "src/parser_bison.c"
    break;

  case 940: /* relational_op: "!="  */
#line 5170 "src/parser_bison.y"
                                                { (yyval.val) = OP_NEQ; }
#line 14654 "src/parser_bison.c"
    break;

  case 941: /* relational_op: "<"  */
#line 5171 "src/parser_bison.y"
                                                { (yyval.val) = OP_LT; }
#line 14660 "src/parser_bison.c"
    break;

  case 942: /* relational_op: ">"  */
#line 5172 "src/parser_bison.y"
                                                { (yyval.val) = OP_GT; }
#line 14666 "src/parser_bison.c"
    break;

  case 943: /* relational_op: ">="  */
#line 5173 "src/parser_bison.y"
                                                { (yyval.val) = OP_GTE; }
#line 14672 "src/parser_bison.c"
    break;

  case 944: /* relational_op: "<="  */
#line 5174 "src/parser_bison.y"
                                                { (yyval.val) = OP_LTE; }
#line 14678 "src/parser_bison.c"
    break;

  case 945: /* relational_op: "!"  */
#line 5175 "src/parser_bison.y"
                                                { (yyval.val) = OP_NEG; }
#line 14684 "src/parser_bison.c"
    break;

  case 946: /* verdict_expr: "accept"  */
#line 5179 "src/parser_bison.y"
                        {
				(yyval.expr) = verdict_expr_alloc(&(yyloc), NF_ACCEPT, NULL);
			}
#line 14692 "src/parser_bison.c"
    break;

  case 947: /* verdict_expr: "drop"  */
#line 5183 "src/parser_bison.y"
                        {
				(yyval.expr) = verdict_expr_alloc(&(yyloc), NF_DROP, NULL);
			}
#line 14700 "src/parser_bison.c"
    break;

  case 948: /* verdict_expr: "continue"  */
#line 5187 "src/parser_bison.y"
                        {
				(yyval.expr) = verdict_expr_alloc(&(yyloc), NFT_CONTINUE, NULL);
			}
#line 14708 "src/parser_bison.c"
    break;

  case 949: /* verdict_expr: "jump" chain_expr  */
#line 5191 "src/parser_bison.y"
                        {
				(yyval.expr) = verdict_expr_alloc(&(yyloc), NFT_JUMP, (yyvsp[0].expr));
			}
#line 14716 "src/parser_bison.c"
    break;

  case 950: /* verdict_expr: "goto" chain_expr  */
#line 5195 "src/parser_bison.y"
                        {
				(yyval.expr) = verdict_expr_alloc(&(yyloc), NFT_GOTO, (yyvsp[0].expr));
			}
#line 14724 "src/parser_bison.c"
    break;

  case 951: /* verdict_expr: "return"  */
#line 5199 "src/parser_bison.y"
                        {
				(yyval.expr) = verdict_expr_alloc(&(yyloc), NFT_RETURN, NULL);
			}
#line 14732 "src/parser_bison.c"
    break;

  case 953: /* chain_expr: identifier  */
#line 5206 "src/parser_bison.y"
                        {
				(yyval.expr) = constant_expr_alloc(&(yyloc), &string_type,
							 BYTEORDER_HOST_ENDIAN,
							 strlen((yyvsp[0].string)) * BITS_PER_BYTE,
							 (yyvsp[0].string));
				free_const((yyvsp[0].string));
			}
#line 14744 "src/parser_bison.c"
    break;

  case 954: /* meta_expr: "meta" meta_key close_scope_meta  */
#line 5216 "src/parser_bison.y"
                        {
				(yyval.expr) = meta_expr_alloc(&(yyloc), (yyvsp[-1].val));
			}
#line 14752 "src/parser_bison.c"
    break;

  case 955: /* meta_expr: meta_key_unqualified  */
#line 5220 "src/parser_bison.y"
                        {
				(yyval.expr) = meta_expr_alloc(&(yyloc), (yyvsp[0].val));
			}
#line 14760 "src/parser_bison.c"
    break;

  case 956: /* meta_expr: "meta" "string" close_scope_meta  */
#line 5224 "src/parser_bison.y"
                        {
				struct error_record *erec;
				unsigned int key;

				erec = meta_key_parse(&(yyloc), (yyvsp[-1].string), &key);
				free_const((yyvsp[-1].string));
				if (erec != NULL) {
					erec_queue(erec, state->msgs);
					YYERROR;
				}

				(yyval.expr) = meta_expr_alloc(&(yyloc), key);
			}
#line 14778 "src/parser_bison.c"
    break;

  case 959: /* meta_key_qualified: "length"  */
#line 5243 "src/parser_bison.y"
                                                { (yyval.val) = NFT_META_LEN; }
#line 14784 "src/parser_bison.c"
    break;

  case 960: /* meta_key_qualified: "protocol"  */
#line 5244 "src/parser_bison.y"
                                                { (yyval.val) = NFT_META_PROTOCOL; }
#line 14790 "src/parser_bison.c"
    break;

  case 961: /* meta_key_qualified: "priority"  */
#line 5245 "src/parser_bison.y"
                                                { (yyval.val) = NFT_META_PRIORITY; }
#line 14796 "src/parser_bison.c"
    break;

  case 962: /* meta_key_qualified: "random"  */
#line 5246 "src/parser_bison.y"
                                                { (yyval.val) = NFT_META_PRANDOM; }
#line 14802 "src/parser_bison.c"
    break;

  case 963: /* meta_key_qualified: "secmark" close_scope_secmark  */
#line 5247 "src/parser_bison.y"
                                                            { (yyval.val) = NFT_META_SECMARK; }
#line 14808 "src/parser_bison.c"
    break;

  case 964: /* meta_key_unqualified: "mark"  */
#line 5250 "src/parser_bison.y"
                                                { (yyval.val) = NFT_META_MARK; }
#line 14814 "src/parser_bison.c"
    break;

  case 965: /* meta_key_unqualified: "iif"  */
#line 5251 "src/parser_bison.y"
                                                { (yyval.val) = NFT_META_IIF; }
#line 14820 "src/parser_bison.c"
    break;

  case 966: /* meta_key_unqualified: "iifname"  */
#line 5252 "src/parser_bison.y"
                                                { (yyval.val) = NFT_META_IIFNAME; }
#line 14826 "src/parser_bison.c"
    break;

  case 967: /* meta_key_unqualified: "iiftype"  */
#line 5253 "src/parser_bison.y"
                                                { (yyval.val) = NFT_META_IIFTYPE; }
#line 14832 "src/parser_bison.c"
    break;

  case 968: /* meta_key_unqualified: "oif"  */
#line 5254 "src/parser_bison.y"
                                                { (yyval.val) = NFT_META_OIF; }
#line 14838 "src/parser_bison.c"
    break;

  case 969: /* meta_key_unqualified: "oifname"  */
#line 5255 "src/parser_bison.y"
                                                { (yyval.val) = NFT_META_OIFNAME; }
#line 14844 "src/parser_bison.c"
    break;

  case 970: /* meta_key_unqualified: "oiftype"  */
#line 5256 "src/parser_bison.y"
                                                { (yyval.val) = NFT_META_OIFTYPE; }
#line 14850 "src/parser_bison.c"
    break;

  case 971: /* meta_key_unqualified: "skuid"  */
#line 5257 "src/parser_bison.y"
                                                { (yyval.val) = NFT_META_SKUID; }
#line 14856 "src/parser_bison.c"
    break;

  case 972: /* meta_key_unqualified: "skgid"  */
#line 5258 "src/parser_bison.y"
                                                { (yyval.val) = NFT_META_SKGID; }
#line 14862 "src/parser_bison.c"
    break;

  case 973: /* meta_key_unqualified: "nftrace"  */
#line 5259 "src/parser_bison.y"
                                                { (yyval.val) = NFT_META_NFTRACE; }
#line 14868 "src/parser_bison.c"
    break;

  case 974: /* meta_key_unqualified: "rtclassid"  */
#line 5260 "src/parser_bison.y"
                                                { (yyval.val) = NFT_META_RTCLASSID; }
#line 14874 "src/parser_bison.c"
    break;

  case 975: /* meta_key_unqualified: "ibriport"  */
#line 5261 "src/parser_bison.y"
                                                { (yyval.val) = NFT_META_BRI_IIFNAME; }
#line 14880 "src/parser_bison.c"
    break;

  case 976: /* meta_key_unqualified: "obriport"  */
#line 5262 "src/parser_bison.y"
                                                { (yyval.val) = NFT_META_BRI_OIFNAME; }
#line 14886 "src/parser_bison.c"
    break;

  case 977: /* meta_key_unqualified: "ibrname"  */
#line 5263 "src/parser_bison.y"
                                                { (yyval.val) = NFT_META_BRI_IIFNAME; }
#line 14892 "src/parser_bison.c"
    break;

  case 978: /* meta_key_unqualified: "obrname"  */
#line 5264 "src/parser_bison.y"
                                                { (yyval.val) = NFT_META_BRI_OIFNAME; }
#line 14898 "src/parser_bison.c"
    break;

  case 979: /* meta_key_unqualified: "pkttype"  */
#line 5265 "src/parser_bison.y"
                                                { (yyval.val) = NFT_META_PKTTYPE; }
#line 14904 "src/parser_bison.c"
    break;

  case 980: /* meta_key_unqualified: "cpu"  */
#line 5266 "src/parser_bison.y"
                                                { (yyval.val) = NFT_META_CPU; }
#line 14910 "src/parser_bison.c"
    break;

  case 981: /* meta_key_unqualified: "iifgroup"  */
#line 5267 "src/parser_bison.y"
                                                { (yyval.val) = NFT_META_IIFGROUP; }
#line 14916 "src/parser_bison.c"
    break;

  case 982: /* meta_key_unqualified: "oifgroup"  */
#line 5268 "src/parser_bison.y"
                                                { (yyval.val) = NFT_META_OIFGROUP; }
#line 14922 "src/parser_bison.c"
    break;

  case 983: /* meta_key_unqualified: "cgroup"  */
#line 5269 "src/parser_bison.y"
                                                { (yyval.val) = NFT_META_CGROUP; }
#line 14928 "src/parser_bison.c"
    break;

  case 984: /* meta_key_unqualified: "ipsec" close_scope_ipsec  */
#line 5270 "src/parser_bison.y"
                                                          { (yyval.val) = NFT_META_SECPATH; }
#line 14934 "src/parser_bison.c"
    break;

  case 985: /* meta_key_unqualified: "time"  */
#line 5271 "src/parser_bison.y"
                                                { (yyval.val) = NFT_META_TIME_NS; }
#line 14940 "src/parser_bison.c"
    break;

  case 986: /* meta_key_unqualified: "day"  */
#line 5272 "src/parser_bison.y"
                                                { (yyval.val) = NFT_META_TIME_DAY; }
#line 14946 "src/parser_bison.c"
    break;

  case 987: /* meta_key_unqualified: "hour"  */
#line 5273 "src/parser_bison.y"
                                                { (yyval.val) = NFT_META_TIME_HOUR; }
#line 14952 "src/parser_bison.c"
    break;

  case 988: /* meta_stmt: "meta" meta_key "set" stmt_expr close_scope_meta  */
#line 5277 "src/parser_bison.y"
                        {
				switch ((yyvsp[-3].val)) {
				case NFT_META_SECMARK:
					switch ((yyvsp[-1].expr)->etype) {
					case EXPR_CT:
						(yyval.stmt) = meta_stmt_alloc(&(yyloc), (yyvsp[-3].val), (yyvsp[-1].expr));
						break;
					default:
						(yyval.stmt) = objref_stmt_alloc(&(yyloc));
						(yyval.stmt)->objref.type = NFT_OBJECT_SECMARK;
						(yyval.stmt)->objref.expr = (yyvsp[-1].expr);
						break;
					}
					break;
				default:
					(yyval.stmt) = meta_stmt_alloc(&(yyloc), (yyvsp[-3].val), (yyvsp[-1].expr));
					break;
				}
			}
#line 14976 "src/parser_bison.c"
    break;

  case 989: /* meta_stmt: meta_key_unqualified "set" stmt_expr  */
#line 5297 "src/parser_bison.y"
                        {
				(yyval.stmt) = meta_stmt_alloc(&(yyloc), (yyvsp[-2].val), (yyvsp[0].expr));
			}
#line 14984 "src/parser_bison.c"
    break;

  case 990: /* meta_stmt: "meta" "string" "set" stmt_expr close_scope_meta  */
#line 5301 "src/parser_bison.y"
                        {
				struct error_record *erec;
				unsigned int key;

				erec = meta_key_parse(&(yyloc), (yyvsp[-3].string), &key);
				free_const((yyvsp[-3].string));
				if (erec != NULL) {
					erec_queue(erec, state->msgs);
					expr_free((yyvsp[-1].expr));
					YYERROR;
				}

				(yyval.stmt) = meta_stmt_alloc(&(yyloc), key, (yyvsp[-1].expr));
			}
#line 15003 "src/parser_bison.c"
    break;

  case 991: /* meta_stmt: "notrack"  */
#line 5316 "src/parser_bison.y"
                        {
				(yyval.stmt) = notrack_stmt_alloc(&(yyloc));
			}
#line 15011 "src/parser_bison.c"
    break;

  case 992: /* meta_stmt: "flow" "offload" "@" string close_scope_at  */
#line 5320 "src/parser_bison.y"
                        {
				(yyval.stmt) = flow_offload_stmt_alloc(&(yyloc), (yyvsp[-1].string));
			}
#line 15019 "src/parser_bison.c"
    break;

  case 993: /* meta_stmt: "flow" "add" "@" string close_scope_at  */
#line 5324 "src/parser_bison.y"
                        {
				(yyval.stmt) = flow_offload_stmt_alloc(&(yyloc), (yyvsp[-1].string));
			}
#line 15027 "src/parser_bison.c"
    break;

  case 994: /* socket_expr: "socket" socket_key close_scope_socket  */
#line 5330 "src/parser_bison.y"
                        {
				(yyval.expr) = socket_expr_alloc(&(yyloc), (yyvsp[-1].val), 0);
			}
#line 15035 "src/parser_bison.c"
    break;

  case 995: /* socket_expr: "socket" "cgroupv2" "level" "number" close_scope_socket  */
#line 5334 "src/parser_bison.y"
                        {
				(yyval.expr) = socket_expr_alloc(&(yyloc), NFT_SOCKET_CGROUPV2, (yyvsp[-1].val));
			}
#line 15043 "src/parser_bison.c"
    break;

  case 996: /* socket_key: "transparent"  */
#line 5339 "src/parser_bison.y"
                                                { (yyval.val) = NFT_SOCKET_TRANSPARENT; }
#line 15049 "src/parser_bison.c"
    break;

  case 997: /* socket_key: "mark"  */
#line 5340 "src/parser_bison.y"
                                                { (yyval.val) = NFT_SOCKET_MARK; }
#line 15055 "src/parser_bison.c"
    break;

  case 998: /* socket_key: "wildcard"  */
#line 5341 "src/parser_bison.y"
                                                { (yyval.val) = NFT_SOCKET_WILDCARD; }
#line 15061 "src/parser_bison.c"
    break;

  case 999: /* offset_opt: %empty  */
#line 5344 "src/parser_bison.y"
                                                { (yyval.val) = 0; }
#line 15067 "src/parser_bison.c"
    break;

  case 1000: /* offset_opt: "offset" "number"  */
#line 5345 "src/parser_bison.y"
                                                { (yyval.val) = (yyvsp[0].val); }
#line 15073 "src/parser_bison.c"
    break;

  case 1001: /* numgen_type: "inc"  */
#line 5348 "src/parser_bison.y"
                                                { (yyval.val) = NFT_NG_INCREMENTAL; }
#line 15079 "src/parser_bison.c"
    break;

  case 1002: /* numgen_type: "random"  */
#line 5349 "src/parser_bison.y"
                                                { (yyval.val) = NFT_NG_RANDOM; }
#line 15085 "src/parser_bison.c"
    break;

  case 1003: /* numgen_expr: "numgen" numgen_type "mod" "number" offset_opt close_scope_numgen  */
#line 5353 "src/parser_bison.y"
                        {
				(yyval.expr) = numgen_expr_alloc(&(yyloc), (yyvsp[-4].val), (yyvsp[-2].val), (yyvsp[-1].val));
			}
#line 15093 "src/parser_bison.c"
    break;

  case 1004: /* xfrm_spnum: "spnum" "number"  */
#line 5358 "src/parser_bison.y"
                                            { (yyval.val) = (yyvsp[0].val); }
#line 15099 "src/parser_bison.c"
    break;

  case 1005: /* xfrm_spnum: %empty  */
#line 5359 "src/parser_bison.y"
                                            { (yyval.val) = 0; }
#line 15105 "src/parser_bison.c"
    break;

  case 1006: /* xfrm_dir: "in"  */
#line 5362 "src/parser_bison.y"
                                        { (yyval.val) = XFRM_POLICY_IN; }
#line 15111 "src/parser_bison.c"
    break;

  case 1007: /* xfrm_dir: "out"  */
#line 5363 "src/parser_bison.y"
                                        { (yyval.val) = XFRM_POLICY_OUT; }
#line 15117 "src/parser_bison.c"
    break;

  case 1008: /* xfrm_state_key: "spi"  */
#line 5366 "src/parser_bison.y"
                                    { (yyval.val) = NFT_XFRM_KEY_SPI; }
#line 15123 "src/parser_bison.c"
    break;

  case 1009: /* xfrm_state_key: "reqid"  */
#line 5367 "src/parser_bison.y"
                                      { (yyval.val) = NFT_XFRM_KEY_REQID; }
#line 15129 "src/parser_bison.c"
    break;

  case 1010: /* xfrm_state_proto_key: "daddr"  */
#line 5370 "src/parser_bison.y"
                                                { (yyval.val) = NFT_XFRM_KEY_DADDR_IP4; }
#line 15135 "src/parser_bison.c"
    break;

  case 1011: /* xfrm_state_proto_key: "saddr"  */
#line 5371 "src/parser_bison.y"
                                                { (yyval.val) = NFT_XFRM_KEY_SADDR_IP4; }
#line 15141 "src/parser_bison.c"
    break;

  case 1012: /* xfrm_expr: "ipsec" xfrm_dir xfrm_spnum xfrm_state_key close_scope_ipsec  */
#line 5375 "src/parser_bison.y"
                        {
				if ((yyvsp[-2].val) > 255) {
					erec_queue(error(&(yylsp[-2]), "value too large"), state->msgs);
					YYERROR;
				}
				(yyval.expr) = xfrm_expr_alloc(&(yyloc), (yyvsp[-3].val), (yyvsp[-2].val), (yyvsp[-1].val));
			}
#line 15153 "src/parser_bison.c"
    break;

  case 1013: /* xfrm_expr: "ipsec" xfrm_dir xfrm_spnum nf_key_proto xfrm_state_proto_key close_scope_ipsec  */
#line 5383 "src/parser_bison.y"
                        {
				enum nft_xfrm_keys xfrmk = (yyvsp[-1].val);

				switch ((yyvsp[-2].val)) {
				case NFPROTO_IPV4:
					break;
				case NFPROTO_IPV6:
					if ((yyvsp[-1].val) == NFT_XFRM_KEY_SADDR_IP4)
						xfrmk = NFT_XFRM_KEY_SADDR_IP6;
					else if ((yyvsp[-1].val) == NFT_XFRM_KEY_DADDR_IP4)
						xfrmk = NFT_XFRM_KEY_DADDR_IP6;
					break;
				default:
					YYERROR;
					break;
				}

				if ((yyvsp[-3].val) > 255) {
					erec_queue(error(&(yylsp[-3]), "value too large"), state->msgs);
					YYERROR;
				}

				(yyval.expr) = xfrm_expr_alloc(&(yyloc), (yyvsp[-4].val), (yyvsp[-3].val), xfrmk);
			}
#line 15182 "src/parser_bison.c"
    break;

  case 1014: /* hash_expr: "jhash" expr "mod" "number" "seed" "number" offset_opt close_scope_hash  */
#line 5410 "src/parser_bison.y"
                        {
				(yyval.expr) = hash_expr_alloc(&(yyloc), (yyvsp[-4].val), true, (yyvsp[-2].val), (yyvsp[-1].val), NFT_HASH_JENKINS);
				(yyval.expr)->hash.expr = (yyvsp[-6].expr);
			}
#line 15191 "src/parser_bison.c"
    break;

  case 1015: /* hash_expr: "jhash" expr "mod" "number" offset_opt close_scope_hash  */
#line 5415 "src/parser_bison.y"
                        {
				(yyval.expr) = hash_expr_alloc(&(yyloc), (yyvsp[-2].val), false, 0, (yyvsp[-1].val), NFT_HASH_JENKINS);
				(yyval.expr)->hash.expr = (yyvsp[-4].expr);
			}
#line 15200 "src/parser_bison.c"
    break;

  case 1016: /* hash_expr: "symhash" "mod" "number" offset_opt close_scope_hash  */
#line 5420 "src/parser_bison.y"
                        {
				(yyval.expr) = hash_expr_alloc(&(yyloc), (yyvsp[-2].val), false, 0, (yyvsp[-1].val), NFT_HASH_SYM);
			}
#line 15208 "src/parser_bison.c"
    break;

  case 1017: /* nf_key_proto: "ip" close_scope_ip  */
#line 5425 "src/parser_bison.y"
                                                       { (yyval.val) = NFPROTO_IPV4; }
#line 15214 "src/parser_bison.c"
    break;

  case 1018: /* nf_key_proto: "ip6" close_scope_ip6  */
#line 5426 "src/parser_bison.y"
                                                        { (yyval.val) = NFPROTO_IPV6; }
#line 15220 "src/parser_bison.c"
    break;

  case 1019: /* rt_expr: "rt" rt_key close_scope_rt  */
#line 5430 "src/parser_bison.y"
                        {
				(yyval.expr) = rt_expr_alloc(&(yyloc), (yyvsp[-1].val), true);
			}
#line 15228 "src/parser_bison.c"
    break;

  case 1020: /* rt_expr: "rt" nf_key_proto rt_key close_scope_rt  */
#line 5434 "src/parser_bison.y"
                        {
				enum nft_rt_keys rtk = (yyvsp[-1].val);

				switch ((yyvsp[-2].val)) {
				case NFPROTO_IPV4:
					break;
				case NFPROTO_IPV6:
					if ((yyvsp[-1].val) == NFT_RT_NEXTHOP4)
						rtk = NFT_RT_NEXTHOP6;
					break;
				default:
					YYERROR;
					break;
				}

				(yyval.expr) = rt_expr_alloc(&(yyloc), rtk, false);
			}
#line 15250 "src/parser_bison.c"
    break;

  case 1021: /* rt_key: "classid"  */
#line 5453 "src/parser_bison.y"
                                                { (yyval.val) = NFT_RT_CLASSID; }
#line 15256 "src/parser_bison.c"
    break;

  case 1022: /* rt_key: "nexthop"  */
#line 5454 "src/parser_bison.y"
                                                { (yyval.val) = NFT_RT_NEXTHOP4; }
#line 15262 "src/parser_bison.c"
    break;

  case 1023: /* rt_key: "mtu"  */
#line 5455 "src/parser_bison.y"
                                                { (yyval.val) = NFT_RT_TCPMSS; }
#line 15268 "src/parser_bison.c"
    break;

  case 1024: /* rt_key: "ipsec" close_scope_ipsec  */
#line 5456 "src/parser_bison.y"
                                                          { (yyval.val) = NFT_RT_XFRM; }
#line 15274 "src/parser_bison.c"
    break;

  case 1025: /* ct_expr: "ct" ct_key close_scope_ct  */
#line 5460 "src/parser_bison.y"
                        {
				(yyval.expr) = ct_expr_alloc(&(yyloc), (yyvsp[-1].val), -1);
			}
#line 15282 "src/parser_bison.c"
    break;

  case 1026: /* ct_expr: "ct" ct_dir ct_key_dir close_scope_ct  */
#line 5464 "src/parser_bison.y"
                        {
				(yyval.expr) = ct_expr_alloc(&(yyloc), (yyvsp[-1].val), (yyvsp[-2].val));
			}
#line 15290 "src/parser_bison.c"
    break;

  case 1027: /* ct_expr: "ct" ct_dir ct_key_proto_field close_scope_ct  */
#line 5468 "src/parser_bison.y"
                        {
				(yyval.expr) = ct_expr_alloc(&(yyloc), (yyvsp[-1].val), (yyvsp[-2].val));
			}
#line 15298 "src/parser_bison.c"
    break;

  case 1028: /* ct_dir: "original"  */
#line 5473 "src/parser_bison.y"
                                                { (yyval.val) = IP_CT_DIR_ORIGINAL; }
#line 15304 "src/parser_bison.c"
    break;

  case 1029: /* ct_dir: "reply"  */
#line 5474 "src/parser_bison.y"
                                                { (yyval.val) = IP_CT_DIR_REPLY; }
#line 15310 "src/parser_bison.c"
    break;

  case 1030: /* ct_key: "l3proto"  */
#line 5477 "src/parser_bison.y"
                                                { (yyval.val) = NFT_CT_L3PROTOCOL; }
#line 15316 "src/parser_bison.c"
    break;

  case 1031: /* ct_key: "protocol"  */
#line 5478 "src/parser_bison.y"
                                                { (yyval.val) = NFT_CT_PROTOCOL; }
#line 15322 "src/parser_bison.c"
    break;

  case 1032: /* ct_key: "mark"  */
#line 5479 "src/parser_bison.y"
                                                { (yyval.val) = NFT_CT_MARK; }
#line 15328 "src/parser_bison.c"
    break;

  case 1033: /* ct_key: "state"  */
#line 5480 "src/parser_bison.y"
                                                { (yyval.val) = NFT_CT_STATE; }
#line 15334 "src/parser_bison.c"
    break;

  case 1034: /* ct_key: "direction"  */
#line 5481 "src/parser_bison.y"
                                                { (yyval.val) = NFT_CT_DIRECTION; }
#line 15340 "src/parser_bison.c"
    break;

  case 1035: /* ct_key: "status"  */
#line 5482 "src/parser_bison.y"
                                                { (yyval.val) = NFT_CT_STATUS; }
#line 15346 "src/parser_bison.c"
    break;

  case 1036: /* ct_key: "expiration"  */
#line 5483 "src/parser_bison.y"
                                                { (yyval.val) = NFT_CT_EXPIRATION; }
#line 15352 "src/parser_bison.c"
    break;

  case 1037: /* ct_key: "helper"  */
#line 5484 "src/parser_bison.y"
                                                { (yyval.val) = NFT_CT_HELPER; }
#line 15358 "src/parser_bison.c"
    break;

  case 1038: /* ct_key: "saddr"  */
#line 5485 "src/parser_bison.y"
                                                { (yyval.val) = NFT_CT_SRC; }
#line 15364 "src/parser_bison.c"
    break;

  case 1039: /* ct_key: "daddr"  */
#line 5486 "src/parser_bison.y"
                                                { (yyval.val) = NFT_CT_DST; }
#line 15370 "src/parser_bison.c"
    break;

  case 1040: /* ct_key: "proto-src"  */
#line 5487 "src/parser_bison.y"
                                                { (yyval.val) = NFT_CT_PROTO_SRC; }
#line 15376 "src/parser_bison.c"
    break;

  case 1041: /* ct_key: "proto-dst"  */
#line 5488 "src/parser_bison.y"
                                                { (yyval.val) = NFT_CT_PROTO_DST; }
#line 15382 "src/parser_bison.c"
    break;

  case 1042: /* ct_key: "label"  */
#line 5489 "src/parser_bison.y"
                                                { (yyval.val) = NFT_CT_LABELS; }
#line 15388 "src/parser_bison.c"
    break;

  case 1043: /* ct_key: "event"  */
#line 5490 "src/parser_bison.y"
                                                { (yyval.val) = NFT_CT_EVENTMASK; }
#line 15394 "src/parser_bison.c"
    break;

  case 1044: /* ct_key: "secmark" close_scope_secmark  */
#line 5491 "src/parser_bison.y"
                                                            { (yyval.val) = NFT_CT_SECMARK; }
#line 15400 "src/parser_bison.c"
    break;

  case 1045: /* ct_key: "id"  */
#line 5492 "src/parser_bison.y"
                                                { (yyval.val) = NFT_CT_ID; }
#line 15406 "src/parser_bison.c"
    break;

  case 1047: /* ct_key_dir: "saddr"  */
#line 5496 "src/parser_bison.y"
                                                { (yyval.val) = NFT_CT_SRC; }
#line 15412 "src/parser_bison.c"
    break;

  case 1048: /* ct_key_dir: "daddr"  */
#line 5497 "src/parser_bison.y"
                                                { (yyval.val) = NFT_CT_DST; }
#line 15418 "src/parser_bison.c"
    break;

  case 1049: /* ct_key_dir: "l3proto"  */
#line 5498 "src/parser_bison.y"
                                                { (yyval.val) = NFT_CT_L3PROTOCOL; }
#line 15424 "src/parser_bison.c"
    break;

  case 1050: /* ct_key_dir: "protocol"  */
#line 5499 "src/parser_bison.y"
                                                { (yyval.val) = NFT_CT_PROTOCOL; }
#line 15430 "src/parser_bison.c"
    break;

  case 1051: /* ct_key_dir: "proto-src"  */
#line 5500 "src/parser_bison.y"
                                                { (yyval.val) = NFT_CT_PROTO_SRC; }
#line 15436 "src/parser_bison.c"
    break;

  case 1052: /* ct_key_dir: "proto-dst"  */
#line 5501 "src/parser_bison.y"
                                                { (yyval.val) = NFT_CT_PROTO_DST; }
#line 15442 "src/parser_bison.c"
    break;

  case 1054: /* ct_key_proto_field: "ip" "saddr" close_scope_ip  */
#line 5505 "src/parser_bison.y"
                                                               { (yyval.val) = NFT_CT_SRC_IP; }
#line 15448 "src/parser_bison.c"
    break;

  case 1055: /* ct_key_proto_field: "ip" "daddr" close_scope_ip  */
#line 5506 "src/parser_bison.y"
                                                               { (yyval.val) = NFT_CT_DST_IP; }
#line 15454 "src/parser_bison.c"
    break;

  case 1056: /* ct_key_proto_field: "ip6" "saddr" close_scope_ip6  */
#line 5507 "src/parser_bison.y"
                                                                { (yyval.val) = NFT_CT_SRC_IP6; }
#line 15460 "src/parser_bison.c"
    break;

  case 1057: /* ct_key_proto_field: "ip6" "daddr" close_scope_ip6  */
#line 5508 "src/parser_bison.y"
                                                                { (yyval.val) = NFT_CT_DST_IP6; }
#line 15466 "src/parser_bison.c"
    break;

  case 1058: /* ct_key_dir_optional: "bytes"  */
#line 5511 "src/parser_bison.y"
                                                { (yyval.val) = NFT_CT_BYTES; }
#line 15472 "src/parser_bison.c"
    break;

  case 1059: /* ct_key_dir_optional: "packets"  */
#line 5512 "src/parser_bison.y"
                                                { (yyval.val) = NFT_CT_PKTS; }
#line 15478 "src/parser_bison.c"
    break;

  case 1060: /* ct_key_dir_optional: "avgpkt"  */
#line 5513 "src/parser_bison.y"
                                                { (yyval.val) = NFT_CT_AVGPKT; }
#line 15484 "src/parser_bison.c"
    break;

  case 1061: /* ct_key_dir_optional: "zone"  */
#line 5514 "src/parser_bison.y"
                                                { (yyval.val) = NFT_CT_ZONE; }
#line 15490 "src/parser_bison.c"
    break;

  case 1064: /* list_stmt_expr: symbol_stmt_expr "comma" symbol_stmt_expr  */
#line 5522 "src/parser_bison.y"
                        {
				(yyval.expr) = list_expr_alloc(&(yyloc));
				compound_expr_add((yyval.expr), (yyvsp[-2].expr));
				compound_expr_add((yyval.expr), (yyvsp[0].expr));
			}
#line 15500 "src/parser_bison.c"
    break;

  case 1065: /* list_stmt_expr: list_stmt_expr "comma" symbol_stmt_expr  */
#line 5528 "src/parser_bison.y"
                        {
				(yyvsp[-2].expr)->location = (yyloc);
				compound_expr_add((yyvsp[-2].expr), (yyvsp[0].expr));
				(yyval.expr) = (yyvsp[-2].expr);
			}
#line 15510 "src/parser_bison.c"
    break;

  case 1066: /* ct_stmt: "ct" ct_key "set" stmt_expr close_scope_ct  */
#line 5536 "src/parser_bison.y"
                        {
				switch ((yyvsp[-3].val)) {
				case NFT_CT_HELPER:
					(yyval.stmt) = objref_stmt_alloc(&(yyloc));
					(yyval.stmt)->objref.type = NFT_OBJECT_CT_HELPER;
					(yyval.stmt)->objref.expr = (yyvsp[-1].expr);
					break;
				default:
					(yyval.stmt) = ct_stmt_alloc(&(yyloc), (yyvsp[-3].val), -1, (yyvsp[-1].expr));
					break;
				}
			}
#line 15527 "src/parser_bison.c"
    break;

  case 1067: /* ct_stmt: "ct" ct_dir ct_key_dir_optional "set" stmt_expr close_scope_ct  */
#line 5549 "src/parser_bison.y"
                        {
				(yyval.stmt) = ct_stmt_alloc(&(yyloc), (yyvsp[-3].val), (yyvsp[-4].val), (yyvsp[-1].expr));
			}
#line 15535 "src/parser_bison.c"
    break;

  case 1068: /* payload_stmt: payload_expr "set" stmt_expr  */
#line 5555 "src/parser_bison.y"
                        {
				if ((yyvsp[-2].expr)->etype == EXPR_EXTHDR)
					(yyval.stmt) = exthdr_stmt_alloc(&(yyloc), (yyvsp[-2].expr), (yyvsp[0].expr));
				else
					(yyval.stmt) = payload_stmt_alloc(&(yyloc), (yyvsp[-2].expr), (yyvsp[0].expr));
			}
#line 15546 "src/parser_bison.c"
    break;

  case 1091: /* payload_raw_len: "number"  */
#line 5588 "src/parser_bison.y"
                        {
				if ((yyvsp[0].val) > NFT_MAX_EXPR_LEN_BITS) {
					erec_queue(error(&(yylsp[0]), "raw payload length %u exceeds upper limit of %u",
							 (yyvsp[0].val), NFT_MAX_EXPR_LEN_BITS),
						 state->msgs);
					YYERROR;
				}

				if ((yyvsp[0].val) == 0) {
					erec_queue(error(&(yylsp[0]), "raw payload length cannot be 0"), state->msgs);
					YYERROR;
				}

				(yyval.val) = (yyvsp[0].val);
			}
#line 15566 "src/parser_bison.c"
    break;

  case 1092: /* payload_raw_expr: "@" payload_base_spec "comma" "number" "comma" payload_raw_len close_scope_at  */
#line 5606 "src/parser_bison.y"
                        {
				(yyval.expr) = payload_expr_alloc(&(yyloc), NULL, 0);
				payload_init_raw((yyval.expr), (yyvsp[-5].val), (yyvsp[-3].val), (yyvsp[-1].val));
				(yyval.expr)->byteorder		= BYTEORDER_BIG_ENDIAN;
				(yyval.expr)->payload.is_raw	= true;
			}
#line 15577 "src/parser_bison.c"
    break;

  case 1093: /* payload_base_spec: "ll"  */
#line 5614 "src/parser_bison.y"
                                                { (yyval.val) = PROTO_BASE_LL_HDR; }
#line 15583 "src/parser_bison.c"
    break;

  case 1094: /* payload_base_spec: "nh"  */
#line 5615 "src/parser_bison.y"
                                                { (yyval.val) = PROTO_BASE_NETWORK_HDR; }
#line 15589 "src/parser_bison.c"
    break;

  case 1095: /* payload_base_spec: "th" close_scope_th  */
#line 5616 "src/parser_bison.y"
                                                                { (yyval.val) = PROTO_BASE_TRANSPORT_HDR; }
#line 15595 "src/parser_bison.c"
    break;

  case 1096: /* payload_base_spec: "string"  */
#line 5618 "src/parser_bison.y"
                        {
				if (!strcmp((yyvsp[0].string), "ih")) {
					(yyval.val) = PROTO_BASE_INNER_HDR;
				} else {
					erec_queue(error(&(yylsp[0]), "unknown raw payload base"), state->msgs);
					free_const((yyvsp[0].string));
					YYERROR;
				}
				free_const((yyvsp[0].string));
			}
#line 15610 "src/parser_bison.c"
    break;

  case 1097: /* eth_hdr_expr: "ether" eth_hdr_field close_scope_eth  */
#line 5631 "src/parser_bison.y"
                        {
				(yyval.expr) = payload_expr_alloc(&(yyloc), &proto_eth, (yyvsp[-1].val));
			}
#line 15618 "src/parser_bison.c"
    break;

  case 1098: /* eth_hdr_field: "saddr"  */
#line 5636 "src/parser_bison.y"
                                                { (yyval.val) = ETHHDR_SADDR; }
#line 15624 "src/parser_bison.c"
    break;

  case 1099: /* eth_hdr_field: "daddr"  */
#line 5637 "src/parser_bison.y"
                                                { (yyval.val) = ETHHDR_DADDR; }
#line 15630 "src/parser_bison.c"
    break;

  case 1100: /* eth_hdr_field: "type" close_scope_type  */
#line 5638 "src/parser_bison.y"
                                                                        { (yyval.val) = ETHHDR_TYPE; }
#line 15636 "src/parser_bison.c"
    break;

  case 1101: /* vlan_hdr_expr: "vlan" vlan_hdr_field close_scope_vlan  */
#line 5642 "src/parser_bison.y"
                        {
				(yyval.expr) = payload_expr_alloc(&(yyloc), &proto_vlan, (yyvsp[-1].val));
			}
#line 15644 "src/parser_bison.c"
    break;

  case 1102: /* vlan_hdr_field: "id"  */
#line 5647 "src/parser_bison.y"
                                                { (yyval.val) = VLANHDR_VID; }
#line 15650 "src/parser_bison.c"
    break;

  case 1103: /* vlan_hdr_field: "cfi"  */
#line 5648 "src/parser_bison.y"
                                                { (yyval.val) = VLANHDR_CFI; }
#line 15656 "src/parser_bison.c"
    break;

  case 1104: /* vlan_hdr_field: "dei"  */
#line 5649 "src/parser_bison.y"
                                                { (yyval.val) = VLANHDR_DEI; }
#line 15662 "src/parser_bison.c"
    break;

  case 1105: /* vlan_hdr_field: "pcp"  */
#line 5650 "src/parser_bison.y"
                                                { (yyval.val) = VLANHDR_PCP; }
#line 15668 "src/parser_bison.c"
    break;

  case 1106: /* vlan_hdr_field: "type" close_scope_type  */
#line 5651 "src/parser_bison.y"
                                                                        { (yyval.val) = VLANHDR_TYPE; }
#line 15674 "src/parser_bison.c"
    break;

  case 1107: /* arp_hdr_expr: "arp" arp_hdr_field close_scope_arp  */
#line 5655 "src/parser_bison.y"
                        {
				(yyval.expr) = payload_expr_alloc(&(yyloc), &proto_arp, (yyvsp[-1].val));
			}
#line 15682 "src/parser_bison.c"
    break;

  case 1108: /* arp_hdr_field: "htype"  */
#line 5660 "src/parser_bison.y"
                                                { (yyval.val) = ARPHDR_HRD; }
#line 15688 "src/parser_bison.c"
    break;

  case 1109: /* arp_hdr_field: "ptype"  */
#line 5661 "src/parser_bison.y"
                                                { (yyval.val) = ARPHDR_PRO; }
#line 15694 "src/parser_bison.c"
    break;

  case 1110: /* arp_hdr_field: "hlen"  */
#line 5662 "src/parser_bison.y"
                                                { (yyval.val) = ARPHDR_HLN; }
#line 15700 "src/parser_bison.c"
    break;

  case 1111: /* arp_hdr_field: "plen"  */
#line 5663 "src/parser_bison.y"
                                                { (yyval.val) = ARPHDR_PLN; }
#line 15706 "src/parser_bison.c"
    break;

  case 1112: /* arp_hdr_field: "operation"  */
#line 5664 "src/parser_bison.y"
                                                { (yyval.val) = ARPHDR_OP; }
#line 15712 "src/parser_bison.c"
    break;

  case 1113: /* arp_hdr_field: "saddr" "ether" close_scope_eth  */
#line 5665 "src/parser_bison.y"
                                                                { (yyval.val) = ARPHDR_SADDR_ETHER; }
#line 15718 "src/parser_bison.c"
    break;

  case 1114: /* arp_hdr_field: "daddr" "ether" close_scope_eth  */
#line 5666 "src/parser_bison.y"
                                                                { (yyval.val) = ARPHDR_DADDR_ETHER; }
#line 15724 "src/parser_bison.c"
    break;

  case 1115: /* arp_hdr_field: "saddr" "ip" close_scope_ip  */
#line 5667 "src/parser_bison.y"
                                                                { (yyval.val) = ARPHDR_SADDR_IP; }
#line 15730 "src/parser_bison.c"
    break;

  case 1116: /* arp_hdr_field: "daddr" "ip" close_scope_ip  */
#line 5668 "src/parser_bison.y"
                                                                { (yyval.val) = ARPHDR_DADDR_IP; }
#line 15736 "src/parser_bison.c"
    break;

  case 1117: /* ip_hdr_expr: "ip" ip_hdr_field close_scope_ip  */
#line 5672 "src/parser_bison.y"
                        {
				(yyval.expr) = payload_expr_alloc(&(yyloc), &proto_ip, (yyvsp[-1].val));
			}
#line 15744 "src/parser_bison.c"
    break;

  case 1118: /* ip_hdr_expr: "ip" "option" ip_option_type ip_option_field close_scope_ip  */
#line 5676 "src/parser_bison.y"
                        {
				(yyval.expr) = ipopt_expr_alloc(&(yyloc), (yyvsp[-2].val), (yyvsp[-1].val));
				if (!(yyval.expr)) {
					erec_queue(error(&(yylsp[-4]), "unknown ip option type/field"), state->msgs);
					YYERROR;
				}

				if ((yyvsp[-1].val) == IPOPT_FIELD_TYPE)
					(yyval.expr)->exthdr.flags = NFT_EXTHDR_F_PRESENT;
			}
#line 15759 "src/parser_bison.c"
    break;

  case 1119: /* ip_hdr_expr: "ip" "option" ip_option_type close_scope_ip  */
#line 5687 "src/parser_bison.y"
                        {
				(yyval.expr) = ipopt_expr_alloc(&(yyloc), (yyvsp[-1].val), IPOPT_FIELD_TYPE);
				(yyval.expr)->exthdr.flags = NFT_EXTHDR_F_PRESENT;
			}
#line 15768 "src/parser_bison.c"
    break;

  case 1120: /* ip_hdr_field: "version"  */
#line 5693 "src/parser_bison.y"
                                                { (yyval.val) = IPHDR_VERSION; }
#line 15774 "src/parser_bison.c"
    break;

  case 1121: /* ip_hdr_field: "hdrlength"  */
#line 5694 "src/parser_bison.y"
                                                { (yyval.val) = IPHDR_HDRLENGTH; }
#line 15780 "src/parser_bison.c"
    break;

  case 1122: /* ip_hdr_field: "dscp"  */
#line 5695 "src/parser_bison.y"
                                                { (yyval.val) = IPHDR_DSCP; }
#line 15786 "src/parser_bison.c"
    break;

  case 1123: /* ip_hdr_field: "ecn"  */
#line 5696 "src/parser_bison.y"
                                                { (yyval.val) = IPHDR_ECN; }
#line 15792 "src/parser_bison.c"
    break;

  case 1124: /* ip_hdr_field: "length"  */
#line 5697 "src/parser_bison.y"
                                                { (yyval.val) = IPHDR_LENGTH; }
#line 15798 "src/parser_bison.c"
    break;

  case 1125: /* ip_hdr_field: "id"  */
#line 5698 "src/parser_bison.y"
                                                { (yyval.val) = IPHDR_ID; }
#line 15804 "src/parser_bison.c"
    break;

  case 1126: /* ip_hdr_field: "frag-off"  */
#line 5699 "src/parser_bison.y"
                                                { (yyval.val) = IPHDR_FRAG_OFF; }
#line 15810 "src/parser_bison.c"
    break;

  case 1127: /* ip_hdr_field: "ttl"  */
#line 5700 "src/parser_bison.y"
                                                { (yyval.val) = IPHDR_TTL; }
#line 15816 "src/parser_bison.c"
    break;

  case 1128: /* ip_hdr_field: "protocol"  */
#line 5701 "src/parser_bison.y"
                                                { (yyval.val) = IPHDR_PROTOCOL; }
#line 15822 "src/parser_bison.c"
    break;

  case 1129: /* ip_hdr_field: "checksum"  */
#line 5702 "src/parser_bison.y"
                                                { (yyval.val) = IPHDR_CHECKSUM; }
#line 15828 "src/parser_bison.c"
    break;

  case 1130: /* ip_hdr_field: "saddr"  */
#line 5703 "src/parser_bison.y"
                                                { (yyval.val) = IPHDR_SADDR; }
#line 15834 "src/parser_bison.c"
    break;

  case 1131: /* ip_hdr_field: "daddr"  */
#line 5704 "src/parser_bison.y"
                                                { (yyval.val) = IPHDR_DADDR; }
#line 15840 "src/parser_bison.c"
    break;

  case 1132: /* ip_option_type: "lsrr"  */
#line 5707 "src/parser_bison.y"
                                                { (yyval.val) = IPOPT_LSRR; }
#line 15846 "src/parser_bison.c"
    break;

  case 1133: /* ip_option_type: "rr"  */
#line 5708 "src/parser_bison.y"
                                                { (yyval.val) = IPOPT_RR; }
#line 15852 "src/parser_bison.c"
    break;

  case 1134: /* ip_option_type: "ssrr"  */
#line 5709 "src/parser_bison.y"
                                                { (yyval.val) = IPOPT_SSRR; }
#line 15858 "src/parser_bison.c"
    break;

  case 1135: /* ip_option_type: "ra"  */
#line 5710 "src/parser_bison.y"
                                                { (yyval.val) = IPOPT_RA; }
#line 15864 "src/parser_bison.c"
    break;

  case 1136: /* ip_option_field: "type" close_scope_type  */
#line 5713 "src/parser_bison.y"
                                                                        { (yyval.val) = IPOPT_FIELD_TYPE; }
#line 15870 "src/parser_bison.c"
    break;

  case 1137: /* ip_option_field: "length"  */
#line 5714 "src/parser_bison.y"
                                                { (yyval.val) = IPOPT_FIELD_LENGTH; }
#line 15876 "src/parser_bison.c"
    break;

  case 1138: /* ip_option_field: "value"  */
#line 5715 "src/parser_bison.y"
                                                { (yyval.val) = IPOPT_FIELD_VALUE; }
#line 15882 "src/parser_bison.c"
    break;

  case 1139: /* ip_option_field: "ptr"  */
#line 5716 "src/parser_bison.y"
                                                { (yyval.val) = IPOPT_FIELD_PTR; }
#line 15888 "src/parser_bison.c"
    break;

  case 1140: /* ip_option_field: "addr"  */
#line 5717 "src/parser_bison.y"
                                                { (yyval.val) = IPOPT_FIELD_ADDR_0; }
#line 15894 "src/parser_bison.c"
    break;

  case 1141: /* icmp_hdr_expr: "icmp" icmp_hdr_field close_scope_icmp  */
#line 5721 "src/parser_bison.y"
                        {
				(yyval.expr) = payload_expr_alloc(&(yyloc), &proto_icmp, (yyvsp[-1].val));
			}
#line 15902 "src/parser_bison.c"
    break;

  case 1142: /* icmp_hdr_field: "type" close_scope_type  */
#line 5726 "src/parser_bison.y"
                                                                        { (yyval.val) = ICMPHDR_TYPE; }
#line 15908 "src/parser_bison.c"
    break;

  case 1143: /* icmp_hdr_field: "code"  */
#line 5727 "src/parser_bison.y"
                                                { (yyval.val) = ICMPHDR_CODE; }
#line 15914 "src/parser_bison.c"
    break;

  case 1144: /* icmp_hdr_field: "checksum"  */
#line 5728 "src/parser_bison.y"
                                                { (yyval.val) = ICMPHDR_CHECKSUM; }
#line 15920 "src/parser_bison.c"
    break;

  case 1145: /* icmp_hdr_field: "id"  */
#line 5729 "src/parser_bison.y"
                                                { (yyval.val) = ICMPHDR_ID; }
#line 15926 "src/parser_bison.c"
    break;

  case 1146: /* icmp_hdr_field: "seq"  */
#line 5730 "src/parser_bison.y"
                                                { (yyval.val) = ICMPHDR_SEQ; }
#line 15932 "src/parser_bison.c"
    break;

  case 1147: /* icmp_hdr_field: "gateway"  */
#line 5731 "src/parser_bison.y"
                                                { (yyval.val) = ICMPHDR_GATEWAY; }
#line 15938 "src/parser_bison.c"
    break;

  case 1148: /* icmp_hdr_field: "mtu"  */
#line 5732 "src/parser_bison.y"
                                                { (yyval.val) = ICMPHDR_MTU; }
#line 15944 "src/parser_bison.c"
    break;

  case 1149: /* igmp_hdr_expr: "igmp" igmp_hdr_field close_scope_igmp  */
#line 5736 "src/parser_bison.y"
                        {
				(yyval.expr) = payload_expr_alloc(&(yyloc), &proto_igmp, (yyvsp[-1].val));
			}
#line 15952 "src/parser_bison.c"
    break;

  case 1150: /* igmp_hdr_field: "type" close_scope_type  */
#line 5741 "src/parser_bison.y"
                                                                        { (yyval.val) = IGMPHDR_TYPE; }
#line 15958 "src/parser_bison.c"
    break;

  case 1151: /* igmp_hdr_field: "checksum"  */
#line 5742 "src/parser_bison.y"
                                                { (yyval.val) = IGMPHDR_CHECKSUM; }
#line 15964 "src/parser_bison.c"
    break;

  case 1152: /* igmp_hdr_field: "mrt"  */
#line 5743 "src/parser_bison.y"
                                                { (yyval.val) = IGMPHDR_MRT; }
#line 15970 "src/parser_bison.c"
    break;

  case 1153: /* igmp_hdr_field: "group"  */
#line 5744 "src/parser_bison.y"
                                                { (yyval.val) = IGMPHDR_GROUP; }
#line 15976 "src/parser_bison.c"
    break;

  case 1154: /* ip6_hdr_expr: "ip6" ip6_hdr_field close_scope_ip6  */
#line 5748 "src/parser_bison.y"
                        {
				(yyval.expr) = payload_expr_alloc(&(yyloc), &proto_ip6, (yyvsp[-1].val));
			}
#line 15984 "src/parser_bison.c"
    break;

  case 1155: /* ip6_hdr_field: "version"  */
#line 5753 "src/parser_bison.y"
                                                { (yyval.val) = IP6HDR_VERSION; }
#line 15990 "src/parser_bison.c"
    break;

  case 1156: /* ip6_hdr_field: "dscp"  */
#line 5754 "src/parser_bison.y"
                                                { (yyval.val) = IP6HDR_DSCP; }
#line 15996 "src/parser_bison.c"
    break;

  case 1157: /* ip6_hdr_field: "ecn"  */
#line 5755 "src/parser_bison.y"
                                                { (yyval.val) = IP6HDR_ECN; }
#line 16002 "src/parser_bison.c"
    break;

  case 1158: /* ip6_hdr_field: "flowlabel"  */
#line 5756 "src/parser_bison.y"
                                                { (yyval.val) = IP6HDR_FLOWLABEL; }
#line 16008 "src/parser_bison.c"
    break;

  case 1159: /* ip6_hdr_field: "length"  */
#line 5757 "src/parser_bison.y"
                                                { (yyval.val) = IP6HDR_LENGTH; }
#line 16014 "src/parser_bison.c"
    break;

  case 1160: /* ip6_hdr_field: "nexthdr"  */
#line 5758 "src/parser_bison.y"
                                                { (yyval.val) = IP6HDR_NEXTHDR; }
#line 16020 "src/parser_bison.c"
    break;

  case 1161: /* ip6_hdr_field: "hoplimit"  */
#line 5759 "src/parser_bison.y"
                                                { (yyval.val) = IP6HDR_HOPLIMIT; }
#line 16026 "src/parser_bison.c"
    break;

  case 1162: /* ip6_hdr_field: "saddr"  */
#line 5760 "src/parser_bison.y"
                                                { (yyval.val) = IP6HDR_SADDR; }
#line 16032 "src/parser_bison.c"
    break;

  case 1163: /* ip6_hdr_field: "daddr"  */
#line 5761 "src/parser_bison.y"
                                                { (yyval.val) = IP6HDR_DADDR; }
#line 16038 "src/parser_bison.c"
    break;

  case 1164: /* icmp6_hdr_expr: "icmpv6" icmp6_hdr_field close_scope_icmp  */
#line 5764 "src/parser_bison.y"
                        {
				(yyval.expr) = payload_expr_alloc(&(yyloc), &proto_icmp6, (yyvsp[-1].val));
			}
#line 16046 "src/parser_bison.c"
    break;

  case 1165: /* icmp6_hdr_field: "type" close_scope_type  */
#line 5769 "src/parser_bison.y"
                                                                        { (yyval.val) = ICMP6HDR_TYPE; }
#line 16052 "src/parser_bison.c"
    break;

  case 1166: /* icmp6_hdr_field: "code"  */
#line 5770 "src/parser_bison.y"
                                                { (yyval.val) = ICMP6HDR_CODE; }
#line 16058 "src/parser_bison.c"
    break;

  case 1167: /* icmp6_hdr_field: "checksum"  */
#line 5771 "src/parser_bison.y"
                                                { (yyval.val) = ICMP6HDR_CHECKSUM; }
#line 16064 "src/parser_bison.c"
    break;

  case 1168: /* icmp6_hdr_field: "param-problem"  */
#line 5772 "src/parser_bison.y"
                                                { (yyval.val) = ICMP6HDR_PPTR; }
#line 16070 "src/parser_bison.c"
    break;

  case 1169: /* icmp6_hdr_field: "mtu"  */
#line 5773 "src/parser_bison.y"
                                                { (yyval.val) = ICMP6HDR_MTU; }
#line 16076 "src/parser_bison.c"
    break;

  case 1170: /* icmp6_hdr_field: "id"  */
#line 5774 "src/parser_bison.y"
                                                { (yyval.val) = ICMP6HDR_ID; }
#line 16082 "src/parser_bison.c"
    break;

  case 1171: /* icmp6_hdr_field: "seq"  */
#line 5775 "src/parser_bison.y"
                                                { (yyval.val) = ICMP6HDR_SEQ; }
#line 16088 "src/parser_bison.c"
    break;

  case 1172: /* icmp6_hdr_field: "max-delay"  */
#line 5776 "src/parser_bison.y"
                                                { (yyval.val) = ICMP6HDR_MAXDELAY; }
#line 16094 "src/parser_bison.c"
    break;

  case 1173: /* icmp6_hdr_field: "taddr"  */
#line 5777 "src/parser_bison.y"
                                                { (yyval.val) = ICMP6HDR_TADDR; }
#line 16100 "src/parser_bison.c"
    break;

  case 1174: /* icmp6_hdr_field: "daddr"  */
#line 5778 "src/parser_bison.y"
                                                { (yyval.val) = ICMP6HDR_DADDR; }
#line 16106 "src/parser_bison.c"
    break;

  case 1175: /* auth_hdr_expr: "ah" auth_hdr_field close_scope_ah  */
#line 5782 "src/parser_bison.y"
                        {
				(yyval.expr) = payload_expr_alloc(&(yyloc), &proto_ah, (yyvsp[-1].val));
			}
#line 16114 "src/parser_bison.c"
    break;

  case 1176: /* auth_hdr_field: "nexthdr"  */
#line 5787 "src/parser_bison.y"
                                                { (yyval.val) = AHHDR_NEXTHDR; }
#line 16120 "src/parser_bison.c"
    break;

  case 1177: /* auth_hdr_field: "hdrlength"  */
#line 5788 "src/parser_bison.y"
                                                { (yyval.val) = AHHDR_HDRLENGTH; }
#line 16126 "src/parser_bison.c"
    break;

  case 1178: /* auth_hdr_field: "reserved"  */
#line 5789 "src/parser_bison.y"
                                                { (yyval.val) = AHHDR_RESERVED; }
#line 16132 "src/parser_bison.c"
    break;

  case 1179: /* auth_hdr_field: "spi"  */
#line 5790 "src/parser_bison.y"
                                                { (yyval.val) = AHHDR_SPI; }
#line 16138 "src/parser_bison.c"
    break;

  case 1180: /* auth_hdr_field: "seq"  */
#line 5791 "src/parser_bison.y"
                                                { (yyval.val) = AHHDR_SEQUENCE; }
#line 16144 "src/parser_bison.c"
    break;

  case 1181: /* esp_hdr_expr: "esp" esp_hdr_field close_scope_esp  */
#line 5795 "src/parser_bison.y"
                        {
				(yyval.expr) = payload_expr_alloc(&(yyloc), &proto_esp, (yyvsp[-1].val));
			}
#line 16152 "src/parser_bison.c"
    break;

  case 1182: /* esp_hdr_field: "spi"  */
#line 5800 "src/parser_bison.y"
                                                { (yyval.val) = ESPHDR_SPI; }
#line 16158 "src/parser_bison.c"
    break;

  case 1183: /* esp_hdr_field: "seq"  */
#line 5801 "src/parser_bison.y"
                                                { (yyval.val) = ESPHDR_SEQUENCE; }
#line 16164 "src/parser_bison.c"
    break;

  case 1184: /* comp_hdr_expr: "comp" comp_hdr_field close_scope_comp  */
#line 5805 "src/parser_bison.y"
                        {
				(yyval.expr) = payload_expr_alloc(&(yyloc), &proto_comp, (yyvsp[-1].val));
			}
#line 16172 "src/parser_bison.c"
    break;

  case 1185: /* comp_hdr_field: "nexthdr"  */
#line 5810 "src/parser_bison.y"
                                                { (yyval.val) = COMPHDR_NEXTHDR; }
#line 16178 "src/parser_bison.c"
    break;

  case 1186: /* comp_hdr_field: "flags"  */
#line 5811 "src/parser_bison.y"
                                                { (yyval.val) = COMPHDR_FLAGS; }
#line 16184 "src/parser_bison.c"
    break;

  case 1187: /* comp_hdr_field: "cpi"  */
#line 5812 "src/parser_bison.y"
                                                { (yyval.val) = COMPHDR_CPI; }
#line 16190 "src/parser_bison.c"
    break;

  case 1188: /* udp_hdr_expr: "udp" udp_hdr_field close_scope_udp  */
#line 5816 "src/parser_bison.y"
                        {
				(yyval.expr) = payload_expr_alloc(&(yyloc), &proto_udp, (yyvsp[-1].val));
			}
#line 16198 "src/parser_bison.c"
    break;

  case 1189: /* udp_hdr_field: "sport"  */
#line 5821 "src/parser_bison.y"
                                                { (yyval.val) = UDPHDR_SPORT; }
#line 16204 "src/parser_bison.c"
    break;

  case 1190: /* udp_hdr_field: "dport"  */
#line 5822 "src/parser_bison.y"
                                                { (yyval.val) = UDPHDR_DPORT; }
#line 16210 "src/parser_bison.c"
    break;

  case 1191: /* udp_hdr_field: "length"  */
#line 5823 "src/parser_bison.y"
                                                { (yyval.val) = UDPHDR_LENGTH; }
#line 16216 "src/parser_bison.c"
    break;

  case 1192: /* udp_hdr_field: "checksum"  */
#line 5824 "src/parser_bison.y"
                                                { (yyval.val) = UDPHDR_CHECKSUM; }
#line 16222 "src/parser_bison.c"
    break;

  case 1193: /* udplite_hdr_expr: "udplite" udplite_hdr_field close_scope_udplite  */
#line 5828 "src/parser_bison.y"
                        {
				(yyval.expr) = payload_expr_alloc(&(yyloc), &proto_udplite, (yyvsp[-1].val));
			}
#line 16230 "src/parser_bison.c"
    break;

  case 1194: /* udplite_hdr_field: "sport"  */
#line 5833 "src/parser_bison.y"
                                                { (yyval.val) = UDPHDR_SPORT; }
#line 16236 "src/parser_bison.c"
    break;

  case 1195: /* udplite_hdr_field: "dport"  */
#line 5834 "src/parser_bison.y"
                                                { (yyval.val) = UDPHDR_DPORT; }
#line 16242 "src/parser_bison.c"
    break;

  case 1196: /* udplite_hdr_field: "csumcov"  */
#line 5835 "src/parser_bison.y"
                                                { (yyval.val) = UDPHDR_LENGTH; }
#line 16248 "src/parser_bison.c"
    break;

  case 1197: /* udplite_hdr_field: "checksum"  */
#line 5836 "src/parser_bison.y"
                                                { (yyval.val) = UDPHDR_CHECKSUM; }
#line 16254 "src/parser_bison.c"
    break;

  case 1198: /* tcp_hdr_expr: "tcp" tcp_hdr_field  */
#line 5840 "src/parser_bison.y"
                        {
				(yyval.expr) = payload_expr_alloc(&(yyloc), &proto_tcp, (yyvsp[0].val));
			}
#line 16262 "src/parser_bison.c"
    break;

  case 1199: /* tcp_hdr_expr: "tcp" "option" tcp_hdr_option_type  */
#line 5844 "src/parser_bison.y"
                        {
				(yyval.expr) = tcpopt_expr_alloc(&(yyloc), (yyvsp[0].val), TCPOPT_COMMON_KIND);
				(yyval.expr)->exthdr.flags = NFT_EXTHDR_F_PRESENT;
			}
#line 16271 "src/parser_bison.c"
    break;

  case 1200: /* tcp_hdr_expr: "tcp" "option" tcp_hdr_option_kind_and_field  */
#line 5849 "src/parser_bison.y"
                        {
				(yyval.expr) = tcpopt_expr_alloc(&(yyloc), (yyvsp[0].tcp_kind_field).kind, (yyvsp[0].tcp_kind_field).field);
				if ((yyval.expr) == NULL) {
					erec_queue(error(&(yylsp[-2]), "Could not find a tcp option template"), state->msgs);
					YYERROR;
				}
			}
#line 16283 "src/parser_bison.c"
    break;

  case 1201: /* tcp_hdr_expr: "tcp" "option" "@" close_scope_at tcp_hdr_option_type "comma" "number" "comma" payload_raw_len  */
#line 5857 "src/parser_bison.y"
                        {
				(yyval.expr) = tcpopt_expr_alloc(&(yyloc), (yyvsp[-4].val), 0);
				tcpopt_init_raw((yyval.expr), (yyvsp[-4].val), (yyvsp[-2].val), (yyvsp[0].val), 0);
			}
#line 16292 "src/parser_bison.c"
    break;

  case 1221: /* vxlan_hdr_expr: "vxlan" vxlan_hdr_field  */
#line 5889 "src/parser_bison.y"
                        {
				struct expr *expr;

				expr = payload_expr_alloc(&(yyloc), &proto_vxlan, (yyvsp[0].val));
				expr->payload.inner_desc = &proto_vxlan;
				(yyval.expr) = expr;
			}
#line 16304 "src/parser_bison.c"
    break;

  case 1222: /* vxlan_hdr_expr: "vxlan" inner_expr  */
#line 5897 "src/parser_bison.y"
                        {
				(yyval.expr) = (yyvsp[0].expr);
				(yyval.expr)->location = (yyloc);
				(yyval.expr)->payload.inner_desc = &proto_vxlan;
			}
#line 16314 "src/parser_bison.c"
    break;

  case 1223: /* vxlan_hdr_field: "vni"  */
#line 5904 "src/parser_bison.y"
                                                        { (yyval.val) = VXLANHDR_VNI; }
#line 16320 "src/parser_bison.c"
    break;

  case 1224: /* vxlan_hdr_field: "flags"  */
#line 5905 "src/parser_bison.y"
                                                        { (yyval.val) = VXLANHDR_FLAGS; }
#line 16326 "src/parser_bison.c"
    break;

  case 1225: /* geneve_hdr_expr: "geneve" geneve_hdr_field  */
#line 5909 "src/parser_bison.y"
                        {
				struct expr *expr;

				expr = payload_expr_alloc(&(yyloc), &proto_geneve, (yyvsp[0].val));
				expr->payload.inner_desc = &proto_geneve;
				(yyval.expr) = expr;
			}
#line 16338 "src/parser_bison.c"
    break;

  case 1226: /* geneve_hdr_expr: "geneve" inner_expr  */
#line 5917 "src/parser_bison.y"
                        {
				(yyval.expr) = (yyvsp[0].expr);
				(yyval.expr)->location = (yyloc);
				(yyval.expr)->payload.inner_desc = &proto_geneve;
			}
#line 16348 "src/parser_bison.c"
    break;

  case 1227: /* geneve_hdr_field: "vni"  */
#line 5924 "src/parser_bison.y"
                                                        { (yyval.val) = GNVHDR_VNI; }
#line 16354 "src/parser_bison.c"
    break;

  case 1228: /* geneve_hdr_field: "type"  */
#line 5925 "src/parser_bison.y"
                                                        { (yyval.val) = GNVHDR_TYPE; }
#line 16360 "src/parser_bison.c"
    break;

  case 1229: /* gre_hdr_expr: "gre" gre_hdr_field close_scope_gre  */
#line 5929 "src/parser_bison.y"
                        {
				(yyval.expr) = payload_expr_alloc(&(yyloc), &proto_gre, (yyvsp[-1].val));
			}
#line 16368 "src/parser_bison.c"
    break;

  case 1230: /* gre_hdr_expr: "gre" close_scope_gre inner_inet_expr  */
#line 5933 "src/parser_bison.y"
                        {
				(yyval.expr) = (yyvsp[0].expr);
				(yyval.expr)->payload.inner_desc = &proto_gre;
			}
#line 16377 "src/parser_bison.c"
    break;

  case 1231: /* gre_hdr_field: "version"  */
#line 5939 "src/parser_bison.y"
                                                        { (yyval.val) = GREHDR_VERSION;	}
#line 16383 "src/parser_bison.c"
    break;

  case 1232: /* gre_hdr_field: "flags"  */
#line 5940 "src/parser_bison.y"
                                                        { (yyval.val) = GREHDR_FLAGS; }
#line 16389 "src/parser_bison.c"
    break;

  case 1233: /* gre_hdr_field: "protocol"  */
#line 5941 "src/parser_bison.y"
                                                        { (yyval.val) = GREHDR_PROTOCOL; }
#line 16395 "src/parser_bison.c"
    break;

  case 1234: /* gretap_hdr_expr: "gretap" close_scope_gre inner_expr  */
#line 5945 "src/parser_bison.y"
                        {
				(yyval.expr) = (yyvsp[0].expr);
				(yyval.expr)->payload.inner_desc = &proto_gretap;
			}
#line 16404 "src/parser_bison.c"
    break;

  case 1235: /* optstrip_stmt: "reset" "tcp" "option" tcp_hdr_option_type close_scope_tcp  */
#line 5952 "src/parser_bison.y"
                        {
				(yyval.stmt) = optstrip_stmt_alloc(&(yyloc), tcpopt_expr_alloc(&(yyloc),
										(yyvsp[-1].val), TCPOPT_COMMON_KIND));
			}
#line 16413 "src/parser_bison.c"
    break;

  case 1236: /* tcp_hdr_field: "sport"  */
#line 5958 "src/parser_bison.y"
                                                { (yyval.val) = TCPHDR_SPORT; }
#line 16419 "src/parser_bison.c"
    break;

  case 1237: /* tcp_hdr_field: "dport"  */
#line 5959 "src/parser_bison.y"
                                                { (yyval.val) = TCPHDR_DPORT; }
#line 16425 "src/parser_bison.c"
    break;

  case 1238: /* tcp_hdr_field: "seq"  */
#line 5960 "src/parser_bison.y"
                                                { (yyval.val) = TCPHDR_SEQ; }
#line 16431 "src/parser_bison.c"
    break;

  case 1239: /* tcp_hdr_field: "ackseq"  */
#line 5961 "src/parser_bison.y"
                                                { (yyval.val) = TCPHDR_ACKSEQ; }
#line 16437 "src/parser_bison.c"
    break;

  case 1240: /* tcp_hdr_field: "doff"  */
#line 5962 "src/parser_bison.y"
                                                { (yyval.val) = TCPHDR_DOFF; }
#line 16443 "src/parser_bison.c"
    break;

  case 1241: /* tcp_hdr_field: "reserved"  */
#line 5963 "src/parser_bison.y"
                                                { (yyval.val) = TCPHDR_RESERVED; }
#line 16449 "src/parser_bison.c"
    break;

  case 1242: /* tcp_hdr_field: "flags"  */
#line 5964 "src/parser_bison.y"
                                                { (yyval.val) = TCPHDR_FLAGS; }
#line 16455 "src/parser_bison.c"
    break;

  case 1243: /* tcp_hdr_field: "window"  */
#line 5965 "src/parser_bison.y"
                                                { (yyval.val) = TCPHDR_WINDOW; }
#line 16461 "src/parser_bison.c"
    break;

  case 1244: /* tcp_hdr_field: "checksum"  */
#line 5966 "src/parser_bison.y"
                                                { (yyval.val) = TCPHDR_CHECKSUM; }
#line 16467 "src/parser_bison.c"
    break;

  case 1245: /* tcp_hdr_field: "urgptr"  */
#line 5967 "src/parser_bison.y"
                                                { (yyval.val) = TCPHDR_URGPTR; }
#line 16473 "src/parser_bison.c"
    break;

  case 1246: /* tcp_hdr_option_kind_and_field: "mss" tcpopt_field_maxseg  */
#line 5971 "src/parser_bison.y"
                                {
					struct tcp_kind_field kind_field = { .kind = TCPOPT_KIND_MAXSEG, .field = (yyvsp[0].val) };
					(yyval.tcp_kind_field) = kind_field;
				}
#line 16482 "src/parser_bison.c"
    break;

  case 1247: /* tcp_hdr_option_kind_and_field: tcp_hdr_option_sack tcpopt_field_sack  */
#line 5976 "src/parser_bison.y"
                                {
					struct tcp_kind_field kind_field = { .kind = (yyvsp[-1].val), .field = (yyvsp[0].val) };
					(yyval.tcp_kind_field) = kind_field;
				}
#line 16491 "src/parser_bison.c"
    break;

  case 1248: /* tcp_hdr_option_kind_and_field: "window" tcpopt_field_window  */
#line 5981 "src/parser_bison.y"
                                {
					struct tcp_kind_field kind_field = { .kind = TCPOPT_KIND_WINDOW, .field = (yyvsp[0].val) };
					(yyval.tcp_kind_field) = kind_field;
				}
#line 16500 "src/parser_bison.c"
    break;

  case 1249: /* tcp_hdr_option_kind_and_field: "timestamp" tcpopt_field_tsopt  */
#line 5986 "src/parser_bison.y"
                                {
					struct tcp_kind_field kind_field = { .kind = TCPOPT_KIND_TIMESTAMP, .field = (yyvsp[0].val) };
					(yyval.tcp_kind_field) = kind_field;
				}
#line 16509 "src/parser_bison.c"
    break;

  case 1250: /* tcp_hdr_option_kind_and_field: tcp_hdr_option_type "length"  */
#line 5991 "src/parser_bison.y"
                                {
					struct tcp_kind_field kind_field = { .kind = (yyvsp[-1].val), .field = TCPOPT_COMMON_LENGTH };
					(yyval.tcp_kind_field) = kind_field;
				}
#line 16518 "src/parser_bison.c"
    break;

  case 1251: /* tcp_hdr_option_kind_and_field: "mptcp" tcpopt_field_mptcp  */
#line 5996 "src/parser_bison.y"
                                {
					struct tcp_kind_field kind_field = { .kind = TCPOPT_KIND_MPTCP, .field = (yyvsp[0].val) };
					(yyval.tcp_kind_field) = kind_field;
				}
#line 16527 "src/parser_bison.c"
    break;

  case 1252: /* tcp_hdr_option_sack: "sack"  */
#line 6002 "src/parser_bison.y"
                                                { (yyval.val) = TCPOPT_KIND_SACK; }
#line 16533 "src/parser_bison.c"
    break;

  case 1253: /* tcp_hdr_option_sack: "sack0"  */
#line 6003 "src/parser_bison.y"
                                                { (yyval.val) = TCPOPT_KIND_SACK; }
#line 16539 "src/parser_bison.c"
    break;

  case 1254: /* tcp_hdr_option_sack: "sack1"  */
#line 6004 "src/parser_bison.y"
                                                { (yyval.val) = TCPOPT_KIND_SACK1; }
#line 16545 "src/parser_bison.c"
    break;

  case 1255: /* tcp_hdr_option_sack: "sack2"  */
#line 6005 "src/parser_bison.y"
                                                { (yyval.val) = TCPOPT_KIND_SACK2; }
#line 16551 "src/parser_bison.c"
    break;

  case 1256: /* tcp_hdr_option_sack: "sack3"  */
#line 6006 "src/parser_bison.y"
                                                { (yyval.val) = TCPOPT_KIND_SACK3; }
#line 16557 "src/parser_bison.c"
    break;

  case 1257: /* tcp_hdr_option_type: "echo"  */
#line 6009 "src/parser_bison.y"
                                                        { (yyval.val) = TCPOPT_KIND_ECHO; }
#line 16563 "src/parser_bison.c"
    break;

  case 1258: /* tcp_hdr_option_type: "eol"  */
#line 6010 "src/parser_bison.y"
                                                        { (yyval.val) = TCPOPT_KIND_EOL; }
#line 16569 "src/parser_bison.c"
    break;

  case 1259: /* tcp_hdr_option_type: "fastopen"  */
#line 6011 "src/parser_bison.y"
                                                        { (yyval.val) = TCPOPT_KIND_FASTOPEN; }
#line 16575 "src/parser_bison.c"
    break;

  case 1260: /* tcp_hdr_option_type: "md5sig"  */
#line 6012 "src/parser_bison.y"
                                                        { (yyval.val) = TCPOPT_KIND_MD5SIG; }
#line 16581 "src/parser_bison.c"
    break;

  case 1261: /* tcp_hdr_option_type: "mptcp"  */
#line 6013 "src/parser_bison.y"
                                                        { (yyval.val) = TCPOPT_KIND_MPTCP; }
#line 16587 "src/parser_bison.c"
    break;

  case 1262: /* tcp_hdr_option_type: "mss"  */
#line 6014 "src/parser_bison.y"
                                                        { (yyval.val) = TCPOPT_KIND_MAXSEG; }
#line 16593 "src/parser_bison.c"
    break;

  case 1263: /* tcp_hdr_option_type: "nop"  */
#line 6015 "src/parser_bison.y"
                                                        { (yyval.val) = TCPOPT_KIND_NOP; }
#line 16599 "src/parser_bison.c"
    break;

  case 1264: /* tcp_hdr_option_type: "sack-permitted"  */
#line 6016 "src/parser_bison.y"
                                                        { (yyval.val) = TCPOPT_KIND_SACK_PERMITTED; }
#line 16605 "src/parser_bison.c"
    break;

  case 1265: /* tcp_hdr_option_type: "timestamp"  */
#line 6017 "src/parser_bison.y"
                                                        { (yyval.val) = TCPOPT_KIND_TIMESTAMP; }
#line 16611 "src/parser_bison.c"
    break;

  case 1266: /* tcp_hdr_option_type: "window"  */
#line 6018 "src/parser_bison.y"
                                                        { (yyval.val) = TCPOPT_KIND_WINDOW; }
#line 16617 "src/parser_bison.c"
    break;

  case 1267: /* tcp_hdr_option_type: tcp_hdr_option_sack  */
#line 6019 "src/parser_bison.y"
                                                        { (yyval.val) = (yyvsp[0].val); }
#line 16623 "src/parser_bison.c"
    break;

  case 1268: /* tcp_hdr_option_type: "number"  */
#line 6020 "src/parser_bison.y"
                                                        {
				if ((yyvsp[0].val) > 255) {
					erec_queue(error(&(yylsp[0]), "value too large"), state->msgs);
					YYERROR;
				}
				(yyval.val) = (yyvsp[0].val);
			}
#line 16635 "src/parser_bison.c"
    break;

  case 1269: /* tcpopt_field_sack: "left"  */
#line 6029 "src/parser_bison.y"
                                                { (yyval.val) = TCPOPT_SACK_LEFT; }
#line 16641 "src/parser_bison.c"
    break;

  case 1270: /* tcpopt_field_sack: "right"  */
#line 6030 "src/parser_bison.y"
                                                { (yyval.val) = TCPOPT_SACK_RIGHT; }
#line 16647 "src/parser_bison.c"
    break;

  case 1271: /* tcpopt_field_window: "count"  */
#line 6033 "src/parser_bison.y"
                                                { (yyval.val) = TCPOPT_WINDOW_COUNT; }
#line 16653 "src/parser_bison.c"
    break;

  case 1272: /* tcpopt_field_tsopt: "tsval"  */
#line 6036 "src/parser_bison.y"
                                                { (yyval.val) = TCPOPT_TS_TSVAL; }
#line 16659 "src/parser_bison.c"
    break;

  case 1273: /* tcpopt_field_tsopt: "tsecr"  */
#line 6037 "src/parser_bison.y"
                                                { (yyval.val) = TCPOPT_TS_TSECR; }
#line 16665 "src/parser_bison.c"
    break;

  case 1274: /* tcpopt_field_maxseg: "size"  */
#line 6040 "src/parser_bison.y"
                                                { (yyval.val) = TCPOPT_MAXSEG_SIZE; }
#line 16671 "src/parser_bison.c"
    break;

  case 1275: /* tcpopt_field_mptcp: "subtype"  */
#line 6043 "src/parser_bison.y"
                                                { (yyval.val) = TCPOPT_MPTCP_SUBTYPE; }
#line 16677 "src/parser_bison.c"
    break;

  case 1276: /* dccp_hdr_expr: "dccp" dccp_hdr_field close_scope_dccp  */
#line 6047 "src/parser_bison.y"
                        {
				(yyval.expr) = payload_expr_alloc(&(yyloc), &proto_dccp, (yyvsp[-1].val));
			}
#line 16685 "src/parser_bison.c"
    break;

  case 1277: /* dccp_hdr_expr: "dccp" "option" "number" close_scope_dccp  */
#line 6051 "src/parser_bison.y"
                        {
				if ((yyvsp[-1].val) > DCCPOPT_TYPE_MAX) {
					erec_queue(error(&(yylsp[-3]), "value too large"),
						   state->msgs);
					YYERROR;
				}
				(yyval.expr) = dccpopt_expr_alloc(&(yyloc), (yyvsp[-1].val));
			}
#line 16698 "src/parser_bison.c"
    break;

  case 1278: /* dccp_hdr_field: "sport"  */
#line 6061 "src/parser_bison.y"
                                                { (yyval.val) = DCCPHDR_SPORT; }
#line 16704 "src/parser_bison.c"
    break;

  case 1279: /* dccp_hdr_field: "dport"  */
#line 6062 "src/parser_bison.y"
                                                { (yyval.val) = DCCPHDR_DPORT; }
#line 16710 "src/parser_bison.c"
    break;

  case 1280: /* dccp_hdr_field: "type" close_scope_type  */
#line 6063 "src/parser_bison.y"
                                                                        { (yyval.val) = DCCPHDR_TYPE; }
#line 16716 "src/parser_bison.c"
    break;

  case 1281: /* sctp_chunk_type: "data"  */
#line 6066 "src/parser_bison.y"
                                                { (yyval.val) = SCTP_CHUNK_TYPE_DATA; }
#line 16722 "src/parser_bison.c"
    break;

  case 1282: /* sctp_chunk_type: "init"  */
#line 6067 "src/parser_bison.y"
                                                { (yyval.val) = SCTP_CHUNK_TYPE_INIT; }
#line 16728 "src/parser_bison.c"
    break;

  case 1283: /* sctp_chunk_type: "init-ack"  */
#line 6068 "src/parser_bison.y"
                                                { (yyval.val) = SCTP_CHUNK_TYPE_INIT_ACK; }
#line 16734 "src/parser_bison.c"
    break;

  case 1284: /* sctp_chunk_type: "sack"  */
#line 6069 "src/parser_bison.y"
                                                { (yyval.val) = SCTP_CHUNK_TYPE_SACK; }
#line 16740 "src/parser_bison.c"
    break;

  case 1285: /* sctp_chunk_type: "heartbeat"  */
#line 6070 "src/parser_bison.y"
                                                { (yyval.val) = SCTP_CHUNK_TYPE_HEARTBEAT; }
#line 16746 "src/parser_bison.c"
    break;

  case 1286: /* sctp_chunk_type: "heartbeat-ack"  */
#line 6071 "src/parser_bison.y"
                                                { (yyval.val) = SCTP_CHUNK_TYPE_HEARTBEAT_ACK; }
#line 16752 "src/parser_bison.c"
    break;

  case 1287: /* sctp_chunk_type: "abort"  */
#line 6072 "src/parser_bison.y"
                                                { (yyval.val) = SCTP_CHUNK_TYPE_ABORT; }
#line 16758 "src/parser_bison.c"
    break;

  case 1288: /* sctp_chunk_type: "shutdown"  */
#line 6073 "src/parser_bison.y"
                                                { (yyval.val) = SCTP_CHUNK_TYPE_SHUTDOWN; }
#line 16764 "src/parser_bison.c"
    break;

  case 1289: /* sctp_chunk_type: "shutdown-ack"  */
#line 6074 "src/parser_bison.y"
                                                { (yyval.val) = SCTP_CHUNK_TYPE_SHUTDOWN_ACK; }
#line 16770 "src/parser_bison.c"
    break;

  case 1290: /* sctp_chunk_type: "error"  */
#line 6075 "src/parser_bison.y"
                                                { (yyval.val) = SCTP_CHUNK_TYPE_ERROR; }
#line 16776 "src/parser_bison.c"
    break;

  case 1291: /* sctp_chunk_type: "cookie-echo"  */
#line 6076 "src/parser_bison.y"
                                                { (yyval.val) = SCTP_CHUNK_TYPE_COOKIE_ECHO; }
#line 16782 "src/parser_bison.c"
    break;

  case 1292: /* sctp_chunk_type: "cookie-ack"  */
#line 6077 "src/parser_bison.y"
                                                { (yyval.val) = SCTP_CHUNK_TYPE_COOKIE_ACK; }
#line 16788 "src/parser_bison.c"
    break;

  case 1293: /* sctp_chunk_type: "ecne"  */
#line 6078 "src/parser_bison.y"
                                                { (yyval.val) = SCTP_CHUNK_TYPE_ECNE; }
#line 16794 "src/parser_bison.c"
    break;

  case 1294: /* sctp_chunk_type: "cwr"  */
#line 6079 "src/parser_bison.y"
                                                { (yyval.val) = SCTP_CHUNK_TYPE_CWR; }
#line 16800 "src/parser_bison.c"
    break;

  case 1295: /* sctp_chunk_type: "shutdown-complete"  */
#line 6080 "src/parser_bison.y"
                                                  { (yyval.val) = SCTP_CHUNK_TYPE_SHUTDOWN_COMPLETE; }
#line 16806 "src/parser_bison.c"
    break;

  case 1296: /* sctp_chunk_type: "asconf-ack"  */
#line 6081 "src/parser_bison.y"
                                                { (yyval.val) = SCTP_CHUNK_TYPE_ASCONF_ACK; }
#line 16812 "src/parser_bison.c"
    break;

  case 1297: /* sctp_chunk_type: "forward-tsn"  */
#line 6082 "src/parser_bison.y"
                                                { (yyval.val) = SCTP_CHUNK_TYPE_FORWARD_TSN; }
#line 16818 "src/parser_bison.c"
    break;

  case 1298: /* sctp_chunk_type: "asconf"  */
#line 6083 "src/parser_bison.y"
                                                { (yyval.val) = SCTP_CHUNK_TYPE_ASCONF; }
#line 16824 "src/parser_bison.c"
    break;

  case 1299: /* sctp_chunk_common_field: "type" close_scope_type  */
#line 6086 "src/parser_bison.y"
                                                                { (yyval.val) = SCTP_CHUNK_COMMON_TYPE; }
#line 16830 "src/parser_bison.c"
    break;

  case 1300: /* sctp_chunk_common_field: "flags"  */
#line 6087 "src/parser_bison.y"
                                        { (yyval.val) = SCTP_CHUNK_COMMON_FLAGS; }
#line 16836 "src/parser_bison.c"
    break;

  case 1301: /* sctp_chunk_common_field: "length"  */
#line 6088 "src/parser_bison.y"
                                        { (yyval.val) = SCTP_CHUNK_COMMON_LENGTH; }
#line 16842 "src/parser_bison.c"
    break;

  case 1302: /* sctp_chunk_data_field: "tsn"  */
#line 6091 "src/parser_bison.y"
                                        { (yyval.val) = SCTP_CHUNK_DATA_TSN; }
#line 16848 "src/parser_bison.c"
    break;

  case 1303: /* sctp_chunk_data_field: "stream"  */
#line 6092 "src/parser_bison.y"
                                        { (yyval.val) = SCTP_CHUNK_DATA_STREAM; }
#line 16854 "src/parser_bison.c"
    break;

  case 1304: /* sctp_chunk_data_field: "ssn"  */
#line 6093 "src/parser_bison.y"
                                        { (yyval.val) = SCTP_CHUNK_DATA_SSN; }
#line 16860 "src/parser_bison.c"
    break;

  case 1305: /* sctp_chunk_data_field: "ppid"  */
#line 6094 "src/parser_bison.y"
                                        { (yyval.val) = SCTP_CHUNK_DATA_PPID; }
#line 16866 "src/parser_bison.c"
    break;

  case 1306: /* sctp_chunk_init_field: "init-tag"  */
#line 6097 "src/parser_bison.y"
                                                { (yyval.val) = SCTP_CHUNK_INIT_TAG; }
#line 16872 "src/parser_bison.c"
    break;

  case 1307: /* sctp_chunk_init_field: "a-rwnd"  */
#line 6098 "src/parser_bison.y"
                                                { (yyval.val) = SCTP_CHUNK_INIT_RWND; }
#line 16878 "src/parser_bison.c"
    break;

  case 1308: /* sctp_chunk_init_field: "num-outbound-streams"  */
#line 6099 "src/parser_bison.y"
                                                { (yyval.val) = SCTP_CHUNK_INIT_OSTREAMS; }
#line 16884 "src/parser_bison.c"
    break;

  case 1309: /* sctp_chunk_init_field: "num-inbound-streams"  */
#line 6100 "src/parser_bison.y"
                                                { (yyval.val) = SCTP_CHUNK_INIT_ISTREAMS; }
#line 16890 "src/parser_bison.c"
    break;

  case 1310: /* sctp_chunk_init_field: "initial-tsn"  */
#line 6101 "src/parser_bison.y"
                                                { (yyval.val) = SCTP_CHUNK_INIT_TSN; }
#line 16896 "src/parser_bison.c"
    break;

  case 1311: /* sctp_chunk_sack_field: "cum-tsn-ack"  */
#line 6104 "src/parser_bison.y"
                                                { (yyval.val) = SCTP_CHUNK_SACK_CTSN_ACK; }
#line 16902 "src/parser_bison.c"
    break;

  case 1312: /* sctp_chunk_sack_field: "a-rwnd"  */
#line 6105 "src/parser_bison.y"
                                                { (yyval.val) = SCTP_CHUNK_SACK_RWND; }
#line 16908 "src/parser_bison.c"
    break;

  case 1313: /* sctp_chunk_sack_field: "num-gap-ack-blocks"  */
#line 6106 "src/parser_bison.y"
                                                { (yyval.val) = SCTP_CHUNK_SACK_GACK_BLOCKS; }
#line 16914 "src/parser_bison.c"
    break;

  case 1314: /* sctp_chunk_sack_field: "num-dup-tsns"  */
#line 6107 "src/parser_bison.y"
                                                { (yyval.val) = SCTP_CHUNK_SACK_DUP_TSNS; }
#line 16920 "src/parser_bison.c"
    break;

  case 1315: /* sctp_chunk_alloc: sctp_chunk_type  */
#line 6111 "src/parser_bison.y"
                        {
				(yyval.expr) = sctp_chunk_expr_alloc(&(yyloc), (yyvsp[0].val), SCTP_CHUNK_COMMON_TYPE);
				(yyval.expr)->exthdr.flags = NFT_EXTHDR_F_PRESENT;
			}
#line 16929 "src/parser_bison.c"
    break;

  case 1316: /* sctp_chunk_alloc: sctp_chunk_type sctp_chunk_common_field  */
#line 6116 "src/parser_bison.y"
                        {
				(yyval.expr) = sctp_chunk_expr_alloc(&(yyloc), (yyvsp[-1].val), (yyvsp[0].val));
			}
#line 16937 "src/parser_bison.c"
    break;

  case 1317: /* sctp_chunk_alloc: "data" sctp_chunk_data_field  */
#line 6120 "src/parser_bison.y"
                        {
				(yyval.expr) = sctp_chunk_expr_alloc(&(yyloc), SCTP_CHUNK_TYPE_DATA, (yyvsp[0].val));
			}
#line 16945 "src/parser_bison.c"
    break;

  case 1318: /* sctp_chunk_alloc: "init" sctp_chunk_init_field  */
#line 6124 "src/parser_bison.y"
                        {
				(yyval.expr) = sctp_chunk_expr_alloc(&(yyloc), SCTP_CHUNK_TYPE_INIT, (yyvsp[0].val));
			}
#line 16953 "src/parser_bison.c"
    break;

  case 1319: /* sctp_chunk_alloc: "init-ack" sctp_chunk_init_field  */
#line 6128 "src/parser_bison.y"
                        {
				(yyval.expr) = sctp_chunk_expr_alloc(&(yyloc), SCTP_CHUNK_TYPE_INIT_ACK, (yyvsp[0].val));
			}
#line 16961 "src/parser_bison.c"
    break;

  case 1320: /* sctp_chunk_alloc: "sack" sctp_chunk_sack_field  */
#line 6132 "src/parser_bison.y"
                        {
				(yyval.expr) = sctp_chunk_expr_alloc(&(yyloc), SCTP_CHUNK_TYPE_SACK, (yyvsp[0].val));
			}
#line 16969 "src/parser_bison.c"
    break;

  case 1321: /* sctp_chunk_alloc: "shutdown" "cum-tsn-ack"  */
#line 6136 "src/parser_bison.y"
                        {
				(yyval.expr) = sctp_chunk_expr_alloc(&(yyloc), SCTP_CHUNK_TYPE_SHUTDOWN,
							   SCTP_CHUNK_SHUTDOWN_CTSN_ACK);
			}
#line 16978 "src/parser_bison.c"
    break;

  case 1322: /* sctp_chunk_alloc: "ecne" "lowest-tsn"  */
#line 6141 "src/parser_bison.y"
                        {
				(yyval.expr) = sctp_chunk_expr_alloc(&(yyloc), SCTP_CHUNK_TYPE_ECNE,
							   SCTP_CHUNK_ECNE_CWR_MIN_TSN);
			}
#line 16987 "src/parser_bison.c"
    break;

  case 1323: /* sctp_chunk_alloc: "cwr" "lowest-tsn"  */
#line 6146 "src/parser_bison.y"
                        {
				(yyval.expr) = sctp_chunk_expr_alloc(&(yyloc), SCTP_CHUNK_TYPE_CWR,
							   SCTP_CHUNK_ECNE_CWR_MIN_TSN);
			}
#line 16996 "src/parser_bison.c"
    break;

  case 1324: /* sctp_chunk_alloc: "asconf-ack" "seqno"  */
#line 6151 "src/parser_bison.y"
                        {
				(yyval.expr) = sctp_chunk_expr_alloc(&(yyloc), SCTP_CHUNK_TYPE_ASCONF_ACK,
							   SCTP_CHUNK_ASCONF_SEQNO);
			}
#line 17005 "src/parser_bison.c"
    break;

  case 1325: /* sctp_chunk_alloc: "forward-tsn" "new-cum-tsn"  */
#line 6156 "src/parser_bison.y"
                        {
				(yyval.expr) = sctp_chunk_expr_alloc(&(yyloc), SCTP_CHUNK_TYPE_FORWARD_TSN,
							   SCTP_CHUNK_FORWARD_TSN_NCTSN);
			}
#line 17014 "src/parser_bison.c"
    break;

  case 1326: /* sctp_chunk_alloc: "asconf" "seqno"  */
#line 6161 "src/parser_bison.y"
                        {
				(yyval.expr) = sctp_chunk_expr_alloc(&(yyloc), SCTP_CHUNK_TYPE_ASCONF,
							   SCTP_CHUNK_ASCONF_SEQNO);
			}
#line 17023 "src/parser_bison.c"
    break;

  case 1327: /* sctp_hdr_expr: "sctp" sctp_hdr_field close_scope_sctp  */
#line 6168 "src/parser_bison.y"
                        {
				(yyval.expr) = payload_expr_alloc(&(yyloc), &proto_sctp, (yyvsp[-1].val));
			}
#line 17031 "src/parser_bison.c"
    break;

  case 1328: /* sctp_hdr_expr: "sctp" "chunk" sctp_chunk_alloc close_scope_sctp_chunk close_scope_sctp  */
#line 6172 "src/parser_bison.y"
                        {
				(yyval.expr) = (yyvsp[-2].expr);
			}
#line 17039 "src/parser_bison.c"
    break;

  case 1329: /* sctp_hdr_field: "sport"  */
#line 6177 "src/parser_bison.y"
                                                { (yyval.val) = SCTPHDR_SPORT; }
#line 17045 "src/parser_bison.c"
    break;

  case 1330: /* sctp_hdr_field: "dport"  */
#line 6178 "src/parser_bison.y"
                                                { (yyval.val) = SCTPHDR_DPORT; }
#line 17051 "src/parser_bison.c"
    break;

  case 1331: /* sctp_hdr_field: "vtag"  */
#line 6179 "src/parser_bison.y"
                                                { (yyval.val) = SCTPHDR_VTAG; }
#line 17057 "src/parser_bison.c"
    break;

  case 1332: /* sctp_hdr_field: "checksum"  */
#line 6180 "src/parser_bison.y"
                                                { (yyval.val) = SCTPHDR_CHECKSUM; }
#line 17063 "src/parser_bison.c"
    break;

  case 1333: /* th_hdr_expr: "th" th_hdr_field close_scope_th  */
#line 6184 "src/parser_bison.y"
                        {
				(yyval.expr) = payload_expr_alloc(&(yyloc), &proto_th, (yyvsp[-1].val));
				if ((yyval.expr))
					(yyval.expr)->payload.is_raw = true;
			}
#line 17073 "src/parser_bison.c"
    break;

  case 1334: /* th_hdr_field: "sport"  */
#line 6191 "src/parser_bison.y"
                                                { (yyval.val) = THDR_SPORT; }
#line 17079 "src/parser_bison.c"
    break;

  case 1335: /* th_hdr_field: "dport"  */
#line 6192 "src/parser_bison.y"
                                                { (yyval.val) = THDR_DPORT; }
#line 17085 "src/parser_bison.c"
    break;

  case 1344: /* hbh_hdr_expr: "hbh" hbh_hdr_field close_scope_hbh  */
#line 6206 "src/parser_bison.y"
                        {
				(yyval.expr) = exthdr_expr_alloc(&(yyloc), &exthdr_hbh, (yyvsp[-1].val));
			}
#line 17093 "src/parser_bison.c"
    break;

  case 1345: /* hbh_hdr_field: "nexthdr"  */
#line 6211 "src/parser_bison.y"
                                                { (yyval.val) = HBHHDR_NEXTHDR; }
#line 17099 "src/parser_bison.c"
    break;

  case 1346: /* hbh_hdr_field: "hdrlength"  */
#line 6212 "src/parser_bison.y"
                                                { (yyval.val) = HBHHDR_HDRLENGTH; }
#line 17105 "src/parser_bison.c"
    break;

  case 1347: /* rt_hdr_expr: "rt" rt_hdr_field close_scope_rt  */
#line 6216 "src/parser_bison.y"
                        {
				(yyval.expr) = exthdr_expr_alloc(&(yyloc), &exthdr_rt, (yyvsp[-1].val));
			}
#line 17113 "src/parser_bison.c"
    break;

  case 1348: /* rt_hdr_field: "nexthdr"  */
#line 6221 "src/parser_bison.y"
                                                { (yyval.val) = RTHDR_NEXTHDR; }
#line 17119 "src/parser_bison.c"
    break;

  case 1349: /* rt_hdr_field: "hdrlength"  */
#line 6222 "src/parser_bison.y"
                                                { (yyval.val) = RTHDR_HDRLENGTH; }
#line 17125 "src/parser_bison.c"
    break;

  case 1350: /* rt_hdr_field: "type" close_scope_type  */
#line 6223 "src/parser_bison.y"
                                                                        { (yyval.val) = RTHDR_TYPE; }
#line 17131 "src/parser_bison.c"
    break;

  case 1351: /* rt_hdr_field: "seg-left"  */
#line 6224 "src/parser_bison.y"
                                                { (yyval.val) = RTHDR_SEG_LEFT; }
#line 17137 "src/parser_bison.c"
    break;

  case 1352: /* rt0_hdr_expr: "rt0" rt0_hdr_field close_scope_rt  */
#line 6228 "src/parser_bison.y"
                        {
				(yyval.expr) = exthdr_expr_alloc(&(yyloc), &exthdr_rt0, (yyvsp[-1].val));
			}
#line 17145 "src/parser_bison.c"
    break;

  case 1353: /* rt0_hdr_field: "addr" '[' "number" ']'  */
#line 6234 "src/parser_bison.y"
                        {
				(yyval.val) = RT0HDR_ADDR_1 + (yyvsp[-1].val) - 1;
			}
#line 17153 "src/parser_bison.c"
    break;

  case 1354: /* rt2_hdr_expr: "rt2" rt2_hdr_field close_scope_rt  */
#line 6240 "src/parser_bison.y"
                        {
				(yyval.expr) = exthdr_expr_alloc(&(yyloc), &exthdr_rt2, (yyvsp[-1].val));
			}
#line 17161 "src/parser_bison.c"
    break;

  case 1355: /* rt2_hdr_field: "addr"  */
#line 6245 "src/parser_bison.y"
                                                { (yyval.val) = RT2HDR_ADDR; }
#line 17167 "src/parser_bison.c"
    break;

  case 1356: /* rt4_hdr_expr: "srh" rt4_hdr_field close_scope_rt  */
#line 6249 "src/parser_bison.y"
                        {
				(yyval.expr) = exthdr_expr_alloc(&(yyloc), &exthdr_rt4, (yyvsp[-1].val));
			}
#line 17175 "src/parser_bison.c"
    break;

  case 1357: /* rt4_hdr_field: "last-entry"  */
#line 6254 "src/parser_bison.y"
                                                { (yyval.val) = RT4HDR_LASTENT; }
#line 17181 "src/parser_bison.c"
    break;

  case 1358: /* rt4_hdr_field: "flags"  */
#line 6255 "src/parser_bison.y"
                                                { (yyval.val) = RT4HDR_FLAGS; }
#line 17187 "src/parser_bison.c"
    break;

  case 1359: /* rt4_hdr_field: "tag"  */
#line 6256 "src/parser_bison.y"
                                                { (yyval.val) = RT4HDR_TAG; }
#line 17193 "src/parser_bison.c"
    break;

  case 1360: /* rt4_hdr_field: "sid" '[' "number" ']'  */
#line 6258 "src/parser_bison.y"
                        {
				(yyval.val) = RT4HDR_SID_1 + (yyvsp[-1].val) - 1;
			}
#line 17201 "src/parser_bison.c"
    break;

  case 1361: /* frag_hdr_expr: "frag" frag_hdr_field close_scope_frag  */
#line 6264 "src/parser_bison.y"
                        {
				(yyval.expr) = exthdr_expr_alloc(&(yyloc), &exthdr_frag, (yyvsp[-1].val));
			}
#line 17209 "src/parser_bison.c"
    break;

  case 1362: /* frag_hdr_field: "nexthdr"  */
#line 6269 "src/parser_bison.y"
                                                { (yyval.val) = FRAGHDR_NEXTHDR; }
#line 17215 "src/parser_bison.c"
    break;

  case 1363: /* frag_hdr_field: "reserved"  */
#line 6270 "src/parser_bison.y"
                                                { (yyval.val) = FRAGHDR_RESERVED; }
#line 17221 "src/parser_bison.c"
    break;

  case 1364: /* frag_hdr_field: "frag-off"  */
#line 6271 "src/parser_bison.y"
                                                { (yyval.val) = FRAGHDR_FRAG_OFF; }
#line 17227 "src/parser_bison.c"
    break;

  case 1365: /* frag_hdr_field: "reserved2"  */
#line 6272 "src/parser_bison.y"
                                                { (yyval.val) = FRAGHDR_RESERVED2; }
#line 17233 "src/parser_bison.c"
    break;

  case 1366: /* frag_hdr_field: "more-fragments"  */
#line 6273 "src/parser_bison.y"
                                                { (yyval.val) = FRAGHDR_MFRAGS; }
#line 17239 "src/parser_bison.c"
    break;

  case 1367: /* frag_hdr_field: "id"  */
#line 6274 "src/parser_bison.y"
                                                { (yyval.val) = FRAGHDR_ID; }
#line 17245 "src/parser_bison.c"
    break;

  case 1368: /* dst_hdr_expr: "dst" dst_hdr_field close_scope_dst  */
#line 6278 "src/parser_bison.y"
                        {
				(yyval.expr) = exthdr_expr_alloc(&(yyloc), &exthdr_dst, (yyvsp[-1].val));
			}
#line 17253 "src/parser_bison.c"
    break;

  case 1369: /* dst_hdr_field: "nexthdr"  */
#line 6283 "src/parser_bison.y"
                                                { (yyval.val) = DSTHDR_NEXTHDR; }
#line 17259 "src/parser_bison.c"
    break;

  case 1370: /* dst_hdr_field: "hdrlength"  */
#line 6284 "src/parser_bison.y"
                                                { (yyval.val) = DSTHDR_HDRLENGTH; }
#line 17265 "src/parser_bison.c"
    break;

  case 1371: /* mh_hdr_expr: "mh" mh_hdr_field close_scope_mh  */
#line 6288 "src/parser_bison.y"
                        {
				(yyval.expr) = exthdr_expr_alloc(&(yyloc), &exthdr_mh, (yyvsp[-1].val));
			}
#line 17273 "src/parser_bison.c"
    break;

  case 1372: /* mh_hdr_field: "nexthdr"  */
#line 6293 "src/parser_bison.y"
                                                { (yyval.val) = MHHDR_NEXTHDR; }
#line 17279 "src/parser_bison.c"
    break;

  case 1373: /* mh_hdr_field: "hdrlength"  */
#line 6294 "src/parser_bison.y"
                                                { (yyval.val) = MHHDR_HDRLENGTH; }
#line 17285 "src/parser_bison.c"
    break;

  case 1374: /* mh_hdr_field: "type" close_scope_type  */
#line 6295 "src/parser_bison.y"
                                                                        { (yyval.val) = MHHDR_TYPE; }
#line 17291 "src/parser_bison.c"
    break;

  case 1375: /* mh_hdr_field: "reserved"  */
#line 6296 "src/parser_bison.y"
                                                { (yyval.val) = MHHDR_RESERVED; }
#line 17297 "src/parser_bison.c"
    break;

  case 1376: /* mh_hdr_field: "checksum"  */
#line 6297 "src/parser_bison.y"
                                                { (yyval.val) = MHHDR_CHECKSUM; }
#line 17303 "src/parser_bison.c"
    break;

  case 1377: /* exthdr_exists_expr: "exthdr" exthdr_key  */
#line 6301 "src/parser_bison.y"
                        {
				const struct exthdr_desc *desc;

				desc = exthdr_find_proto((yyvsp[0].val));

				/* Assume that NEXTHDR template is always
				 * the first one in list of templates.
				 */
				(yyval.expr) = exthdr_expr_alloc(&(yyloc), desc, 1);
				(yyval.expr)->exthdr.flags = NFT_EXTHDR_F_PRESENT;
			}
#line 17319 "src/parser_bison.c"
    break;

  case 1378: /* exthdr_key: "hbh" close_scope_hbh  */
#line 6314 "src/parser_bison.y"
                                                        { (yyval.val) = IPPROTO_HOPOPTS; }
#line 17325 "src/parser_bison.c"
    break;

  case 1379: /* exthdr_key: "rt" close_scope_rt  */
#line 6315 "src/parser_bison.y"
                                                        { (yyval.val) = IPPROTO_ROUTING; }
#line 17331 "src/parser_bison.c"
    break;

  case 1380: /* exthdr_key: "frag" close_scope_frag  */
#line 6316 "src/parser_bison.y"
                                                                { (yyval.val) = IPPROTO_FRAGMENT; }
#line 17337 "src/parser_bison.c"
    break;

  case 1381: /* exthdr_key: "dst" close_scope_dst  */
#line 6317 "src/parser_bison.y"
                                                        { (yyval.val) = IPPROTO_DSTOPTS; }
#line 17343 "src/parser_bison.c"
    break;

  case 1382: /* exthdr_key: "mh" close_scope_mh  */
#line 6318 "src/parser_bison.y"
                                                        { (yyval.val) = IPPROTO_MH; }
#line 17349 "src/parser_bison.c"
    break;


#line 17353 "src/parser_bison.c"

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;
  *++yylsp = yyloc;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      {
        yypcontext_t yyctx
          = {yyssp, yytoken, &yylloc};
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = yysyntax_error (&yymsg_alloc, &yymsg, &yyctx);
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == -1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = YY_CAST (char *,
                             YYSTACK_ALLOC (YY_CAST (YYSIZE_T, yymsg_alloc)));
            if (yymsg)
              {
                yysyntax_error_status
                  = yysyntax_error (&yymsg_alloc, &yymsg, &yyctx);
                yymsgp = yymsg;
              }
            else
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = YYENOMEM;
              }
          }
        yyerror (&yylloc, nft, scanner, state, yymsgp);
        if (yysyntax_error_status == YYENOMEM)
          YYNOMEM;
      }
    }

  yyerror_range[1] = yylloc;
  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= TOKEN_EOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == TOKEN_EOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval, &yylloc, nft, scanner, state);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;
  ++yynerrs;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;

      yyerror_range[1] = *yylsp;
      yydestruct ("Error: popping",
                  YY_ACCESSING_SYMBOL (yystate), yyvsp, yylsp, nft, scanner, state);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  yyerror_range[2] = yylloc;
  ++yylsp;
  YYLLOC_DEFAULT (*yylsp, yyerror_range, 2);

  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturnlab;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturnlab;


/*-----------------------------------------------------------.
| yyexhaustedlab -- YYNOMEM (memory exhaustion) comes here.  |
`-----------------------------------------------------------*/
yyexhaustedlab:
  yyerror (&yylloc, nft, scanner, state, YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturnlab;


/*----------------------------------------------------------.
| yyreturnlab -- parsing is finished, clean up and return.  |
`----------------------------------------------------------*/
yyreturnlab:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval, &yylloc, nft, scanner, state);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp, yylsp, nft, scanner, state);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
  return yyresult;
}

#line 6321 "src/parser_bison.y"

