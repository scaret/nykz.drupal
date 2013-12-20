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
function _nykzPost_resource_create($boardName, $title, $content, $refilename){
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
      $content_lines = split("\n", $refile['content']);
        foreach($content_lines as $key => $line){
          if($line == "--") break;
          if(substr($line, 0, 6) == ": : : ") continue;
          if( strlen($line) && $key > 3) $recontent .= "\n: ".$line;
        }
    }
  }
  if($recontent) $reply =<<<EOD
【 在 $reauthor 的大作中提到: 】$recontent;
EOD;
  $body =<<<EOD
发信人: $user->name ($user->name), 信区: $boardName
标  题: $title
发信站: 南洋客栈BBS ($date), 南洋客栈客户端

$content
$reply

--

[m[1;36m※ 来源:·南洋客栈客户端[m
EOD;
  if(!$title && $refile){
    $retitle = $refile['index']['title'];
    if(strpos("Re: ", $retitle)) $retitle = $title;
    else $title = "Re: ".$retitle;
  }
  $reid = $refile['index']['id'];
  $filename = nykz_post_create($boardName, $title, $body);
  return $filename;
}