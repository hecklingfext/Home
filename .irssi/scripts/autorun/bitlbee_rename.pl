# this script was written by Timbo on #bitlbee
# http://a.ngus.net/bitlbee_rename.pl

# I just modified it to only match against u\d+ and chat.facebook.com
# and use forname_surname instead of ForenameSurname (my preference :)

# And I modified it to fix the u -> - change that FB made on 13-05-2001 
# AJS

use strict;
use Socket;
use Irssi;
use Irssi::Irc;

my $bitlbeeChannel = "&bitlbee";
my %nicksToRename = ();

sub message_join
{
  # "message join", SERVER_REC, char *channel, char *nick, char *address
  my ($server, $channel, $nick, $address) = @_;
  my $username = substr($address, 0, index($address,'@'));
  my $host = substr($address, index($address,'@')+1);

  if($channel == $bitlbeeChannel && $nick =~ m/^-\d+/ && $host == "chat.facebook.com")
  {
    $nicksToRename{$nick} = $channel;
    $server->command("whois \"$nick\"");
  }
}

sub whois_data
{
  my ($server, $data) = @_;
  my ($me, $nick, $user, $host) = split(" ", $data);

  if (exists($nicksToRename{$nick}))
  {
    my $channel = $nicksToRename{$nick};
    delete($nicksToRename{$nick});

    my $ircname = substr($data, index($data,':')+1);

    $ircname =~ s/ /_/g;
    $ircname =~ s/(-|'|\.)//g;
    $ircname = "x_".$ircname;
    $server->command("msg $channel rename $nick ".lc($ircname));
    $server->command("msg $channel save");
  }
}

Irssi::signal_add_first 'message join' => 'message_join';
Irssi::signal_add_first 'event 311' => 'whois_data';
