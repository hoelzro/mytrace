#!/usr/bin/env perl

use strict;
use warnings;
use feature 'say';

use DBI;

my $dbh = DBI->connect('dbi:mysql:host=127.0.0.1', undef, undef, {
    RaiseError => 1,
    PrintError => 0,
});

my $sth = $dbh->prepare('SHOW DATABASES');

$sth->execute;

while(my $row = $sth->fetch) {
}
