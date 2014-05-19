# *************************************************************************
# Copyright (c) 2009~ , BIGDATA. All Rights Reserved.
# *************************************************************************

# Set BIGDATA local machine specific environment variables here.

# The only required environment variable is JAVA_HOME. All others are
# optional. When running a distributed configuration it is best to set
# JAVA_HOME in this file, so that it is correctly defined on remote nodes.
##

##########################################################################
# A flag variable to avoid duplicated running of this scrpit.
if [ "$BIGDATA_ENV_SET" != "true" ]; then
export BIGDATA_ENV_SET="true"
##########################################################################

# The java implementation to use. Required.
export JAVA_HOME="/usr/lib/jvm/jdk1.6.0_45"

# Include app customized options from the defined file.
if [ "$BIGDATA_COPTS_FILE" != "" ] ; then
  if [ -f "$BIGDATA_COPTS_FILE" ]; then
    . "$BIGDATA_COPTS_FILE"
  fi
fi

# Extra Java CLASSPATH elements.
# The app may customized its specified classpath, setting here appends it.
# export BIGDATA_CLASSPATH=$BIGDATA_HOME/share-conf
if [ "$BIGDATA_CLASSPATH_COPTS" != "" ]; then
  export BIGDATA_CLASSPATH="$BIGDATA_CLASSPATH_COPTS"
fi

# The minimum and maximum size of heap to use, with unit.
export BIGDATA_HEAP_MIN=128m
export BIGDATA_HEAP_MAX=1024m

# The app may customized its heap size, setting here overwrites them.
if [ "$BIGDATA_HEAP_MIN_COPTS" != "" ]; then
  export BIGDATA_HEAP_MIN="$BIGDATA_HEAP_MIN_COPTS"
fi
if [ "$BIGDATA_HEAP_MAX_COPTS" != "" ]; then
  export BIGDATA_HEAP_MAX="$BIGDATA_HEAP_MAX_COPTS"
fi

# Extra Java runtime options.
# The app may customized its specified options, setting here appends it.
export BIGDATA_OPTS="$BIGDATA_COPTS \
                     -server \
                     -XX:+UseParNewGC \
                     -XX:+UseConcMarkSweepGC \
                     -XX:+CMSIncrementalMode \
                     -XX:+CMSParallelRemarkEnabled \
                     -XX:CMSInitiatingOccupancyFraction=70"

# Extra ssh options.  Empty by default.
# export BIGDATA_SSH_OPTS="-o ConnectTimeout=1 -o SendEnv=BIGDATA_CONF_DIR"

# Where log files are stored.  $BIGDATA_HOME/logs by default.
export BIGDATA_LOG_DIR="$BIGDATA_HOME/logs"

# The directory where pid files are stored. $BIGDATA_HOME/pids by default.
export BIGDATA_PID_DIR="$BIGDATA_HOME/pids"

# File naming remote hosts.  $BIGDATA_HOME/conf/nodes by default.
export BIGDATA_NODES="$BIGDATA_HOME/conf/nodes"

# Seconds to sleep between commands on each node.
# This can be useful in large clusters.
# export BIGDATA_NODE_SLEEP=0.1

# A string representing this instance of BIGDATA. $USER by default.
export BIGDATA_IDENT_STRING="$USER"

# The scheduling priority for daemon processes. 0 by default. See 'man nice'.
export BIGDATA_NICENESS=0


##########################################################################
# A flag variable to avoid duplicating run of this scrpit.
fi
##########################################################################
