// event listener
function listen_ajaxURL()
{
    $('a.ajax-url').on('click', function(event){event.preventDefault();});
}
$(document).ready(function(){listen_ajaxURL();});

function ajaxURL(url)
{
    $.ajax({ 	type: 'POST',
                url: url,
                success: function(data)
				{
                    location.reload();
                },
                error: function(data)
				{
                    console.log(data);
                }
            });

    return false;
}
