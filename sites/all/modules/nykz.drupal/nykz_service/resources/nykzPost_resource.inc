<?php
/**
 * 列出南洋客栈帖子列表
 */
function _nykzPost_resource_index($boardName, $start, $count){
  $posts = nykz_post_index($boardName,$start, $count);
  return $posts;
}

/**
 * 获取某个帖子
 */
function _nykzPost_resource_retrieve($boardName, $filename, $format = 'novt'){
  $the_post = nykz_post_load($boardName, $filename, $format);
  return $the_post;
}

/**
 * 发表帖子
 */
function _nykzPost_resource_create($boardName, $title = NULL, $content, $refilename){
  //return array($boardName, $title, $content, $refilename);
  global $user;
  $date = date("Y年m月d日 H:i:s");
  $weekarray=array("日","一","二","三","四","五","六");
  $date .= " 星期".$weekarray[date("w")];
  $reply = "";
  $recontent = "";
  if($refilename){
    $refile = nykz_post_load($boardName, $refilename);
    if($refile){
      if($refile['match']){
        $reauthor = $refile['match']['author']." (".$refile['match']['nick'].")";
      }
      elseif($refile['index']){
        $reauthor = $refile['index']['owner']." (".$refile['index']['owner'].")";
      }
      if(!$title){
        $retitle = $refile['index']['title'];
        if(preg_match("/^Re:/i", $retitle, $matches)) $title = $retitle;
        else $title = "Re: ".$retitle;
      }
      $content_lines = explode("\n", $refile['content']);
        foreach($content_lines as $key => $line){
          if($line == "--") break;
          if(strlen($line) > 250 ) continue;
          if( strlen($line) && $key > 3) $recontent .= "\n: ".$line;
        }
    }
  }
  global $nykz_source_name;
  $ip_address = $_SERVER["REMOTE_ADDR"];
  if($recontent) $reply =<<<EOD
【 在 $reauthor 的大作中提到: 】$recontent
EOD;
  $body =<<<EOD
发信人: $user->name ($user->name), 信区: $boardName
标  题: $title
发信站: 南洋客栈BBS ($date), 南洋客栈客户端

$content
$reply

--

[m[1;36m※ 来源:·$nykz_source_name nykz.net·[FROM: $ip_address][m
EOD;
  if(isset($refile) && isset($refile['index']) && isset($refile['index']['reid']))
    $reid = $refile['index']['reid'];
  else
    $reid = NULL;
  $filename = nykz_post_create($boardName, $title, $body, $user->name, $reid);
  return $filename;
}
