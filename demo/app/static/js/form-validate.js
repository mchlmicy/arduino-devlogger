function validate(form, errorText)
{
    $.ajax(
    {
        data: form.serialize(), // get form data
        type: form.attr('method'), // get POST-type (GET or POST)
        url:  form.attr('action'), // get action
        success: function(data)
        {
            try
            {
                // parse the data
                var json = $.parseJSON(data), errors = json['action-errors'], super_error = json['super-error'], fields = [];

                // remove existing errors
                $(form).find('.has-error').each(function(){$(this).removeClass('has-error');});

                // apply error highlight & get error
                if(!$.isEmptyObject(json['action-errors']))
                {
                    for(field in errors)
                    {
                        fields.push(field);
                        $(form).find("[name='"+field+"']").parent().addClass('has-error');
                    }

                    if(fields.length>1){var error_text = 'Please check your fields and submit again.';} else{var error_text = errors[fields[0]];}
                }
                else
                {
                    var x = 0; while(super_error.fields[x] != null)
                    {
                        var input = $(form).find("[name='"+super_error.fields[x]+"']");
                        if(input.attr('type')!='checkbox'){input.parent().addClass('has-error');}
                        else{input.closest('[class^=col-]').addClass('has-error');}
                        x++;
                    }

                    var error_text = super_error.error
                }

                // apply error & display text
                $(form).find('label.error-text').html(error_text);
                $(errorText).addClass('has-error').css('display', 'inherit');
            }
            catch(err)
            {
                location.reload();
            }
        }
    });

    return false;
}
