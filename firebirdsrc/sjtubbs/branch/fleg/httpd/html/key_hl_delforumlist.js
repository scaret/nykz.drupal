<!--
function killErrors() { return true; } 
window.onerror = killErrors; 

self.focus();

document.onkeydown = kd;

function kd()
{
	if ( window.event.keyCode == 75 && !window.event.ctrlKey && !window.event.altKey )
	{
		//K, ��
		moveup();
	}
	else if ( window.event.keyCode == 74 && !window.event.ctrlKey && !window.event.altKey )
	{
		//J, ��
		movedown();
	}
	else if ( window.event.keyCode == 39 )
	{
		//�ң�������
		viewforum();
	}
	else if ( window.event.keyCode == 37 )
	{
		//��
		//alert("��");
	}
	else if ( window.event.keyCode == 13 )
	{
		//�س���������
		viewforum();
	}
	else if ( window.event.keyCode == 82 && !window.event.ctrlKey && !window.event.altKey )
	{
		//r��������
		viewforum();
	}
	else if ( window.event.keyCode == 79 && !window.event.ctrlKey && !window.event.altKey )
	{
		//O, �ָ�
		returndel();
	}
		else if ( window.event.keyCode == 36 || window.event.keyCode == 48 )
	{
		//Home 0, ������ҳ
		GoToBegin();
	}
	else if ( window.event.keyCode ==35 || window.event.keyCode == 52 )
	{
		//End 4, ����ĩҳ
		GoToEnd();
	}
	else if ( window.event.keyCode == 33 )
	{
		//PageUp, ���Ϸ�ҳ
		PageUp();
	}
	else if ( window.event.keyCode == 34 )
	{
		//PageDown, ���·�ҳ
		PageDown();
	}
	else
	{
		//alert(window.event.keyCode);
	}
}

function movedown()
{
	var item;

	if ( totalid <= currentid )
	{
		//�������βҳ,��ҳ
		if ( start < (total-totalid-1) )
		{
			PageDown();
		}
		return;
	}
	
	//��ǰ��">"ȥ��
	item = "arrow" + currentid;
	document.all(item).innerText = "";
	
	//�ѵ�ǰ�α��ƶ�����һ����
	++currentid;
	item = "arrow" + currentid;
	document.all(item).innerText = ">";
}

function moveup()
{
	var item;

	if ( currentid == 0 )
	{
		//������ҳ����ҳ
		if ( start > 0 )
		{
			PageUp();
		}
		return;
	}

	//��ǰ��">"ȥ��
	item = "arrow" + currentid;
	document.all(item).innerText = "";
	
	//�ѵ�ǰ�α��ƶ�����һ����
	--currentid;
	item = "arrow" + currentid;
	document.all(item).innerText = ">";
}

function viewforum()
{
	location.href('bbscondel?' + arrayid[currentid]);
	foo();
}

function PageDown()
{
	location.href('bbsdocdel?board=' + board + '&start=' + (start+totalid+1));
	foo();
}

function PageUp()
{
	location.href('bbsdocdel?board=' + board + '&start=' + (start-totalid-1));
	foo();
}

function GoToEnd()
{
	location.href('bbsdocdel?currentpoint=' + (totalid-toptotal) + '&board=' + board);
	foo();
}

function GoToBegin()
{
	location.href('bbsdocdel?start=0&board=' + board);
	foo();
}

function returndel()
{
	location.href('bbsdelreturn?' + arrayid[currentid]);
	foo();
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
