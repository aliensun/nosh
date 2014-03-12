README
======
The Foursquare module helps you in using Foursquare in your website. Foursquare
demands an OAUTH implementation in order to get private information about your users, or
to check-in your user at a certain place.

The module handels the authentication with Foursquare for your users.

It does not provide any information coming from foursquare. Getting information
from foursquare is site dependent and should be implemented seperatly. This module is a
helpfull tool.

However, the foursquare-async api is used in this module and can be reused in
order to communicate with Foursquare:

FUTURE IDEAS
============
- A Foursquare module providing basic location information for node with location
- A Foursquare module enabling users to check in on places represented by a node
- A Foursquare module enhancing the users profile information with foursquare data

RESOURCES
=========
Foursquare API documentation: http://groups.google.com/group/foursquare-api/web/api-documentation

foursquare-async api (included in this module): http://github.com/jmathai/foursquare-async

AUTHOR
======
Tom Deryckere
Tom.Deryckere@gmail.com