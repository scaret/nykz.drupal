#!/usr/bin/perl
# ����Ԥ������ soff ��ƣ�soff@263.net
# modified by flyriver, flyriver@happynet.org, 2001.10.21

$HOME="/home/bbs/bbshome";

system("cd $HOME/tmp/; /usr/bin/wget -c -Y off -t 0 http://202.106.184.193/index.html &> /dev/null");
open(IN, "$HOME/tmp/index.html") || die("can't open index.html");
while (<IN>){
    chomp($_);
    $temp=$temp.$_;
};
close(IN);
unlink("$HOME/tmp/index.html");

open(OUT,">$HOME/0Announce/weather") || die("cannot open $HOME/0Announce/weather");
@temp_wh = ($temp=~/<\/table>(.*?)<!--��ʼ���ײ�-->/ig);
$temp = $temp_wh[0];
@temp_wh=($temp=~/<tr align=center>(.*?)<\/tr>/ig);
($sec, $min, $hour, $mday, $mon)=localtime(time);
$newmon = $mon + 1;
$tmp = 0;
$color = 0;
print OUT "\n                      [0;1;40;31m24Сʱ����Ԥ��[m ($newmon��$mday��$hour:$min:$sec)\n\n";
foreach $wh (@temp_wh) {
    $tmp++;
    $str = "";
    @info = ($wh=~/<td>(.*?)<\/td>/ig);
    @wtime = ($wh=~/<td colspan=3>(.*?)<\/td>/ig);
#	if($tmp==1){
#		next;
#	}
    if(($tmp==1) && !($wtime[0] eq "")) {
    print OUT "���������������ө������������������������������ө�������������������������������\n";
    print OUT "��   ʱ   ��  ��[0;1;40;33m         $wtime[0]         [m��[0;1;40;33m         $wtime[1]         [m��\n";
    print OUT "�ĩ������������੤���������Щ��������Щ��������੤���������Щ��������Щ���������\n";
    print OUT "��   ��   ��  �� ����״�� ���������������¶ȩ� ����״�� ���������������¶ȩ�\n";
    print OUT "�ĩ������������੤���������੤�������੤�������੤���������੤�������੤��������\n";
    }

    if(!($info[0] eq "����״��") && !($info[0] eq "") ) {
        $color++;
	if( $info[0] eq "�Ϻ�" )
	{
            $str.="��[1;34;47m".&addspace($info[0], 12)."[0;47m��[1;34m";
            $str.=&addspace($info[1], 10)."[0;47m��[1;34m";
            $str.=&addspace($info[2], 8)."[0;47m��[1;34m";
            $str.=&addspace($info[3], 8)."[0;47m��[1;34m";
            $str.=&addspace($info[4], 10)."[0;47m��[1;34m";
            $str.=&addspace($info[5], 8)."[0;47m��[1;34m";
            $str.=&addspace($info[6], 8)."[m��";
	}
	else
	{
 	       	if ($color % 2 == 0)
        	{
            	$str.="��[44m".&addspace($info[0], 12)."��";
            	$str.=&addspace($info[1], 10)."��";
            	$str.=&addspace($info[2], 8)."��";
            	$str.=&addspace($info[3], 8)."��";
            	$str.=&addspace($info[4], 10)."��";
            	$str.=&addspace($info[5], 8)."��";
            	$str.=&addspace($info[6], 8)."[m��";
        	}
        	else
        	{
            		$str.="��[1;34;46m".&addspace($info[0], 12)."[0;46m��[1;34m";
            		$str.=&addspace($info[1], 10)."[0;46m��[1;34m";
            		$str.=&addspace($info[2], 8)."[0;46m��[1;34m";
            		$str.=&addspace($info[3], 8)."[0;46m��[1;34m";
            		$str.=&addspace($info[4], 10)."[0;46m��[1;34m";
            		$str.=&addspace($info[5], 8)."[0;46m��[1;34m";
            		$str.=&addspace($info[6], 8)."[m��";
        	}
	}
    print OUT "$str\n";
    }
}
print OUT "���������������۩����������۩��������۩��������۩����������۩��������۩���������\n";
close(OUT);
sub addspace {
    my($str, $len) = @_;
    $lenadd=$len-length($str);
    for($i=0; $i<$lenadd; $i++) {
        $str.=" ";
    }
    return $str;
}
