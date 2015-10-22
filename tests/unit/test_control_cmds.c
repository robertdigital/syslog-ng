#include "control_server.h"
#include "testutils.h"

#include "control.c"
#include "hds.h"

void
test_log()
{
  GString *command = g_string_sized_new(128);
  GString *reply;

  g_string_assign(command,"LOG");
  reply = control_connection_message_log(command);
  assert_string(reply->str, "Invalid arguments received, expected at least one argument", "Bad reply");
  g_string_free(reply, TRUE);

  g_string_assign(command,"LOG fakelog");
  reply = control_connection_message_log(command);
  assert_string(reply->str, "Invalid arguments received", "Bad reply");
  g_string_free(reply, TRUE);

  verbose_flag = 0;
  debug_flag = 1;
  trace_flag = 1;
  g_string_assign(command,"LOG VERBOSE");
  reply = control_connection_message_log(command);
  assert_string(reply->str, "VERBOSE=0", "Bad reply");
  g_string_free(reply, TRUE);

  g_string_assign(command,"LOG VERBOSE ON");
  reply = control_connection_message_log(command);
  assert_string(reply->str, "OK", "Bad reply");
  assert_gint(verbose_flag,1,"Flag isn't changed");
  g_string_free(reply, TRUE);

  g_string_assign(command,"LOG VERBOSE OFF");
  reply = control_connection_message_log(command);
  assert_string(reply->str, "OK", "Bad reply");
  assert_gint(verbose_flag,0,"Flag isn't changed");
  g_string_free(reply, TRUE);

  debug_flag = 0;
  verbose_flag = 1;
  trace_flag = 1;
  g_string_assign(command,"LOG DEBUG");
  reply = control_connection_message_log(command);
  assert_string(reply->str, "DEBUG=0", "Bad reply");
  g_string_free(reply, TRUE);

  trace_flag = 0;
  verbose_flag = 1;
  debug_flag = 1;
  g_string_assign(command,"LOG TRACE");
  reply = control_connection_message_log(command);
  assert_string(reply->str, "TRACE=0", "Bad reply");
  g_string_free(reply, TRUE);

  g_string_free(command, TRUE);
  return;
}

void
test_stats()
{
  GString *reply = NULL;
  GString *command = g_string_sized_new(128);
  StatsCounterItem *counter = NULL;

  stats_init();
  stats_register_counter(0, SCS_CENTER, "id", "received", SC_TYPE_PROCESSED, &counter);

  g_string_assign(command,"STATS");

  reply = control_connection_send_stats(command);
  assert_string(reply->str, "SourceName;SourceId;SourceInstance;State;Type;Number\ncenter;id;received;a;processed;0\n", "Bad reply");
  g_string_free(reply, TRUE);

  stats_destroy();
  hds_destroy();
  g_string_free(command, TRUE);
  return;
}

void
test_reset_stats()
{
  GString *reply = NULL;
  GString *command = g_string_sized_new(128);
  StatsCounterItem *counter = NULL;

  stats_init();
  stats_register_counter(0, SCS_CENTER, "id", "received", SC_TYPE_PROCESSED, &counter);
  stats_counter_set(counter, 666);

  g_string_assign(command, "RESET_STATS");
  reply = control_connection_reset_stats(command);
  assert_string(reply->str, "The statistics of syslog-ng have been reset to 0.", "Bad reply");
  g_string_free(reply, TRUE);

  g_string_assign(command, "STATS");
  reply = control_connection_send_stats(command);
  assert_string(reply->str, "SourceName;SourceId;SourceInstance;State;Type;Number\ncenter;id;received;a;processed;0\n", "Bad reply");
  g_string_free(reply, TRUE);

  stats_destroy();
  hds_destroy();
  g_string_free(command, TRUE);
  return;
}

int
main(int argc G_GNUC_UNUSED, char *argv[] G_GNUC_UNUSED)
{
  test_log();
  test_stats();
  test_reset_stats();
  return 0;
}
