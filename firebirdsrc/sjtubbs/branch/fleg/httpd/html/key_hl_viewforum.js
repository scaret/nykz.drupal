<!--
function killErrors() { return true; } 
window.onerror = killErrors; 

self.focus();

document.onkeydown = kd;

function kd()
{
	if ( window.event.keyCode == 75 && !window.event.ctrlKey && !window.event.altKey )
	{
		//k����
		previous();
	}
	else if ( window.event.keyCode == 74 && !window.event.ctrlKey && !window.event.altKey )
	{
		//j����
		next();
	}
	else if ( window.event.keyCode == 37 )
	{
		//��
		returnboard();
	}
	else if ( window.event.keyCode == 82 && !window.event.ctrlKey && !window.event.altKey )
	{
		//r���ظ�����
		reforum();
	}
	else if ( window.event.keyCode == 80 && !window.event.ctrlKey && !window.event.altKey )
	{
		//p, ͬ�����Ķ�
		readsubject();
	}
	else if ( window.event.keyCode == 70 && !window.event.ctrlKey && !window.event.altKey )
	{
		//f, ת��/�Ƽ�
		forward();
	}
	else if ( window.event.keyCode == 90 && !window.event.ctrlKey && !window.event.altKey )
	{
		//z, ת��
		cc();
	}
	else if ( window.event.keyCode == 69 && !window.event.ctrlKey && !window.event.altKey )
	{
		//E, �޸�
		modify();
	}
	else if ( window.event.keyCode == 68 && !window.event.ctrlKey && !window.event.altKey )
	{
		//d, ɾ��
		del();
	}
	else if ( window.event.keyCode == 189 && window.event.shiftKey )
	{
		//Shift + -, ���ò���re
		nore();
	}
	else if ( window.event.keyCode == 72 && !window.event.ctrlKey && !window.event.altKey )
	{
		//H, �鿴���̰���
		showhelp();
	}
	else
	{
		//alert(window.event.keyCode);
	}
}

function next()
{
	if ( nextfilename != "" )
	{
		location.href('bbscon?board=' + board + '&file=' + nextfilename + '&num=' + (start+currentid+1) + '&start=' + start);
		foo();
	}
}

function previous()
{
	if ( prefilename != "" )
	{
		location.href('bbscon?board=' + board + '&file=' + prefilename + '&num=' + (start+currentid-1) + '&start=' + start);
		foo();
	}
}

function reforum()
{
	location.href('bbspst?board=' + board + '&file=' + fileName);
	foo();
}

function readsubject()
{
	location.href('bbstfind?board=' + board + '&title=' + title);
	foo();
}

function returnboard()
{
	location.href('bbsdoc?board=' + board + '&currentpoint=' + currentid + '&start=' + start);
	foo();
}

function forward()
{
	location.href('bbsfwd?board=' + board + '&file=' + filename);
	foo();
}

function cc()
{
	location.href('bbsccc?board=' + board + '&file=' + filename);
	foo();
}

function modify()
{
	location.href('bbsedit?board=' + board + '&file=' + filename);
	foo();
}

function del()
{
	if ( confirm("�����Ҫɾ��������") )
	{
		location.href('bbsdel?board=' + board + '&file=' + filename);
		foo();
	}
}

function nore()
{
	location.href('bbsdocnore?board=' + board + '&file=' + filename + '&start=' + start + '&num=' + (start+currentid));
	foo();
}

function showhelp()
{
	var str	= "[ת��/�Ƽ�F] [ת��Z] [ɾ��D] [�޸�E] [���ÿ�RE����_]\n";
	str	+= "[��һƪK] [������������] [��һƪJ]";
	str	+= "[������R] [ͬ�����Ķ�P]";
	alert(str);
}


function foo()
{
	//����ֹͣ�¼�����
	event.cancelBubble	= true;
	//��β��ٽ����û�������Ϣ�������ɶ�η�������
	document.onkeydown	= foo2;
}

function foo2()
{
	//ɶҲ����
}

-->
