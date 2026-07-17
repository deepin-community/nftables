/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison interface for Yacc-like parsers in C

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

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

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

#line 823 "src/parser_bison.h"

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
